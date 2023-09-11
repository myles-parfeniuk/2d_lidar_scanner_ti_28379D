import sys
import serial
import threading
import time
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QPushButton, QSizePolicy, QVBoxLayout, QMenuBar, QLabel
from PyQt5.QtCore import QTimer
from PyQt5.QtGui import QPalette, QColor
from matplotlib import style

# Define constants for the moving window filter
WINDOW_SIZE = 10  # Adjust this for your desired window size
sample_rate_window = []
elapsed_times_window = []

distance = 0.0  # distance measured by sensor
degrees = 0.0  # absolute position of motor shaft (to be implemented)
sample_count = 0
sample_rate_disp_val = 0
sample_rate_timer_start = 0

# Semaphores
sample_rate_semaphore = threading.Semaphore()
distance_semaphore = threading.Semaphore()
degrees_semaphore = threading.Semaphore()

# open comport
data_stream = serial.Serial('COM6', 3000000)


# retrieves data from comport and formats it for plotting
def read_data():
    # Use global vars
    global distance
    global degrees
    global data_stream
    global sample_count
    global sample_rate_timer_start

    # get initial start time for sample rate calculation
    sample_rate_timer_start = time.perf_counter_ns()

    while True:
        # Read line, decode utf-8 style string, and strip trailing whitespace
        line = data_stream.readline().decode('utf-8').strip()
        # Split line into argument list with space as delimiter
        argument_list = line.split(" ")

        if len(argument_list) == 2:
            try:
                degrees_semaphore.acquire()
                degrees = float(argument_list[0])  # Save degrees
                degrees_semaphore.release()

                distance_semaphore.acquire()
                distance = float(argument_list[1])  # Save distance
                distance_semaphore.release()
                sample_count += 1

                print(distance)

            except ValueError:
                print("Invalid data:", line)

        # ~100ms sampling period to find sample rate
        if (time.perf_counter_ns() - sample_rate_timer_start) > 100000000:
            calc_sample_rate()


# updates line plot figure
def update_plot():
    # Use globals
    global line
    global distance

    # update line data
    distance_semaphore.acquire()
    line.set_ydata([0, distance])
    distance_semaphore.release()

    sample_rate_semaphore.acquire()
    sample_rate_label.setText(f"Sample Rate: {sample_rate_disp_val:.2f} (Hz)")
    sample_rate_semaphore.release()

    # draw canvas
    canvas.draw()
    # run one-shot timer to call update_plot recursively with a 5ms period (200hz canvas refresh rate)
    QTimer.singleShot(5, update_plot)


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


# quits PyQt5 application
def quit():
    global app
    app.quit()


# set the fast render theme for the plot
style.use('fast')

# initialize the PyQt5 Application
app = QApplication(sys.argv)

# create a main window
window = QMainWindow()
window.setWindowTitle("LiDAR Distance Line Plotter")  # window title
window.setGeometry(200, 200, 1200, 600)  # window start location and dimensions

# create plot widget at set it as central widget
plot_widget = QWidget()
window.setCentralWidget(plot_widget)

# create a layout for the plot widget
layout = QVBoxLayout()  # use vertical box layout
plot_widget.setLayout(layout)  # set layout of plot widget
layout.addWidget(plot_widget)

# create a quit button
quit_button = QPushButton("Quit")  # button text
quit_button.clicked.connect(quit)  # connect button to quit() function on click
quit_button.setFixedWidth(60)  # set button width
layout.addWidget(quit_button)  # add button to existing layout

# initialize the Matplotlib figure
fig, ax = plt.subplots()
ax.set_xlim(0, 10)  # set arbitrary x axis values
ax.set_ylim(0, 50)  # set max Y range of line to 50cm

# create an initial line from (5, 0) to (5, distance)
line, = ax.plot([5, 5], [0, distance], color=(0.0, 0.8, 0.0), linewidth=4)

# remove x-axis labels and ticks
ax.set_xticks([])
ax.set_xticklabels([])

# add a y-axis label
ax.set_ylabel("Distance (cm)")
ax.set_title("TF-Mini-S Distance Measured")

# embed the Matplotlib figure within a QWidget
canvas = FigureCanvas(fig)
#  set resizing behaviour such that plot expands when space is available in window
canvas.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
layout.addWidget(canvas)

# add sample rate label
sample_rate_label = QLabel(f"Sample Rate: {sample_rate_disp_val} Hz")
layout.addWidget(sample_rate_label)

# start updating the plot
update_plot()

# start the data reading thread
read_data_thread = threading.Thread(target=read_data)
# use daemon thread such that application can exit even if reading_data_thread is running
read_data_thread.daemon = True
read_data_thread.start()

# display the main window
window.show()

# start the PyQt5 Application gui loop
sys.exit(app.exec_())
