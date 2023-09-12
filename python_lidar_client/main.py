import sys
import serial
import threading
import time
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QPushButton, QSizePolicy, QVBoxLayout, QMenuBar, QLabel, \
    QHBoxLayout
from PyQt5.QtCore import QTimer, Qt
from PyQt5.QtGui import QPalette, QColor
from matplotlib import style


#  Global variables:

# Define constants for the moving window filter
WINDOW_SIZE = 10  # Adjust this for your desired window size
sample_rate_window = []
elapsed_times_window = []

# distances measured by sensor
distance_1 = 0.0
distance_2 = 0.0
# absolute position of motor shaft (to be implemented)
degrees = 0.0
sample_count = 0
# rate at which samples are received from controller by python client
sample_rate_disp_val = 0
sample_rate_timer_start = 0

# whether or not the sensor was blocked by an object in the last second
object_detected = False


# semaphores
sample_rate_semaphore = threading.Semaphore()
distance_1_semaphore = threading.Semaphore()
distance_2_semaphore = threading.Semaphore()
degrees_semaphore = threading.Semaphore()

# retrieves data from comport and formats it for plotting
def read_data():
    # Use global vars
    global distance_1
    global distance_2
    global degrees
    global data_stream
    global sample_count
    global sample_rate_timer_start

    # get initial start time for sample rate calculation
    sample_rate_timer_start = time.perf_counter_ns()

    while True:
        # Read line, decode utf-8 style string, and strip trailing whitespace
        uart_payload = data_stream.readline().decode('utf-8').strip()
        # Split line into argument list with space as delimiter
        argument_list = uart_payload.split(" ")

        if len(argument_list) == 3:
            try:
                # save distance absolute position of motor shaft
                degrees_semaphore.acquire()
                degrees = float(argument_list[0])  # Save degrees
                degrees_semaphore.release()

                # save distance from sensor 1
                distance_1_semaphore.acquire()
                distance_1 = float(argument_list[1])  # Save distance

                if (distance_1 < 50.0) and (distance_1 != 0):
                    show_object_detected()

                distance_1_semaphore.release()

                # save distance from sensor 2
                distance_2_semaphore.acquire()
                distance_2 = float(argument_list[2])

                if (distance_2 < 50.0) and (distance_2 != 0):
                    show_object_detected()

                distance_2_semaphore.release()

                print(str(distance_1) + " " + str(distance_2))

                sample_count += 1

            except ValueError:
                print("Invalid data.")

        # ~100ms sampling period to find sample rate
        if (time.perf_counter_ns() - sample_rate_timer_start) > 100000000:
            calc_sample_rate()


# updates line plot figure
def update_plot():
    # Use globals
    global line_sensor_1
    global line_sensor_2
    global distance_1

    # update line_sensor_1 data
    distance_1_semaphore.acquire()
    line_sensor_1.set_ydata([0, distance_1])
    distance_1_semaphore.release()

    # update line_sensor_2 data
    distance_2_semaphore.acquire()
    line_sensor_2.set_ydata([0, distance_2])
    distance_2_semaphore.release()

    # update sample rate label
    sample_rate_semaphore.acquire()
    sample_rate_label.setText(f"Sample Rate: {sample_rate_disp_val:.2f} (Hz)")
    sample_rate_semaphore.release()

    # draw canvas
    canvas.draw()
    # run one-shot timer to call update_plot recursively with a 5ms period (200hz canvas refresh rate)
    QTimer.singleShot(25, update_plot)


# calculates the sample rate in samples/s (ie the speed at which samples are being received from controller)
def calc_sample_rate():
    global sample_rate_disp_val
    global sample_count
    global sample_rate_timer_start
    global sample_rate_window

    # elapsed time since the last calculation in ns
    elapsed_time = (time.perf_counter_ns() - sample_rate_timer_start)

    # append the current sample rate and time to respective windows
    sample_rate_window.append(sample_count)
    elapsed_times_window.append(elapsed_time)

    # remove the oldest sample rate value if the window sizes exceeded
    if len(sample_rate_window) > WINDOW_SIZE:
        sample_rate_window.pop(0)

    if len(elapsed_times_window) > WINDOW_SIZE:
        elapsed_times_window.pop(0)

    # average windows
    filtered_sample_rate = sum(sample_rate_window) / len(sample_rate_window)
    filtered_elapsed_times = (sum(elapsed_times_window) / len(elapsed_times_window)) / 1e9

    # calc sample rate (average_samples_per_period/average_time_period)
    filtered_sample_rate = filtered_sample_rate / filtered_elapsed_times

    sample_rate_semaphore.acquire()
    # update the display value with new filtered sample rate
    if (len(sample_rate_window) == WINDOW_SIZE and len(elapsed_times_window) == WINDOW_SIZE):
        sample_rate_disp_val = filtered_sample_rate
    sample_rate_semaphore.release()

    # reset sample_count and start time
    sample_count = 0
    sample_rate_timer_start = time.perf_counter_ns()

def show_object_detected():
    global object_detected

    if not object_detected:
        object_detected_label.setText("Object Detected")

    object_detected = True

    timer = threading.Timer(1.0, hide_object_detected)
    timer.start()

def hide_object_detected():
    global object_detected

    if object_detected:

        object_detected = False

        if (distance_2 > 50.0) and (distance_1 > 50.0):
            object_detected_label.clear()  # Clear the label's text




# quits PyQt5 application
def quit():
    global app
    app.quit()


# open comport
data_stream = serial.Serial('COM6', 3100000)

# set the fast render theme for the plot
style.use('fast')

# initialize the PyQt5 Application
app = QApplication(sys.argv)

# create central widget
central_widget = QWidget()

# create central layout
central_layout = QVBoxLayout()

# create a main window
window = QMainWindow()
window.setWindowTitle("LiDAR Distance Line Plotter")
window.setGeometry(200, 200, 1200, 600)

# set the central widget for the main window
window.setCentralWidget(central_widget)

# create a plot and label container widget
plot_widget = QWidget()
plot_layout = QVBoxLayout()  # use vertical box layout
label_widget = QWidget()
label_layout = QHBoxLayout()

# create control container widget
control_widget = QWidget()
control_layout = QVBoxLayout()

# create a quit button
quit_button = QPushButton("Quit")  # button text
quit_button.clicked.connect(quit)  # connect button to quit() function on click
quit_button.setFixedWidth(60)  # set button width
control_layout.addWidget(quit_button)  # add button to existing layout

# initialize the Matplotlib figure
fig, ax = plt.subplots()
ax.set_xlim(0, 10)  # set arbitrary x axis values
ax.set_ylim(0, 50)  # set max Y range of line to 50cm

# create an initial line from (2, 0) to (2, distance_1)
line_sensor_1, = ax.plot([2, 2], [0, distance_1], color=(0.0, 0.8, 0.0), linewidth=8)
# create an initial line from (8, 0) to (8, distance_2)
line_sensor_2, = ax.plot([8, 8], [0, distance_2], color=(0.0, 0.8, 0.0), linewidth=8)

# remove x-axis labels and ticks
ax.set_xticks([])
ax.set_xticklabels([])

# add a y-axis label
ax.set_ylabel("Distance (cm)")
ax.set_title("TF-Mini-S Distance Measured")

ax.grid(True)

# embed the Matplotlib figure within a QWidget
canvas = FigureCanvas(fig)
# set resizing behavior such that the plot expands when space is available in the window
canvas.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)

# create object detected label
object_detected_label = QLabel()
object_detected_label.setStyleSheet("QLabel { color : red; font-size : 20px; border: 2px solid black; padding: 5px; }")
object_detected_label.setAlignment(Qt.AlignCenter)

plot_layout.addWidget(canvas)
plot_layout.addWidget(object_detected_label)
plot_layout.addWidget(label_widget)

# add sample rate label
sample_rate_label = QLabel(f"Sample Rate: {sample_rate_disp_val} Hz", window)
sample_rate_label.setFixedWidth(200)
label_layout.setAlignment(Qt.AlignLeft)
label_layout.addWidget(sample_rate_label)




# assign the plot layout to the plot widget
plot_widget.setLayout(plot_layout)

# assign the label layout to the label widget
label_widget.setLayout(label_layout)

# assign the control layout to control widget
control_widget.setLayout(control_layout)

# add container widgets to the central layout
central_layout.addWidget(control_widget, 1)
central_layout.addWidget(plot_widget, 10)


# set the main layout for the central widget
central_widget.setLayout(central_layout)

# start updating the plot
update_plot()

# start the data reading thread
read_data_thread = threading.Thread(target=read_data)
# use daemon thread so that the application can exit even if reading_data_thread is running
read_data_thread.daemon = True
read_data_thread.start()

# display the main window
window.show()

# start the PyQt5 Application GUI loop
sys.exit(app.exec_())