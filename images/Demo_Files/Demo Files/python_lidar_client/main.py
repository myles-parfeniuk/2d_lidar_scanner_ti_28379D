import sys
import serial
import threading
import time
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QPushButton, QSizePolicy, QVBoxLayout, QMenuBar, QLabel, \
    QHBoxLayout
from PyQt5.QtCore import QTimer, Qt
from PyQt5.QtGui import QPalette, QColor
from matplotlib import style
import math


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

scatter_plot_points = []

sensor_1_dot = None
sensor_2_dot = None

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
    global scatter_plot_points

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
                prev_degrees = degrees
                degrees = math.degrees(float(argument_list[0])/1000.0)  # Save degrees

                if (prev_degrees > degrees):
                    scatter_plot_points.clear()

                degrees_semaphore.release()

                # save distance from sensor 1
                distance_1_semaphore.acquire()
                distance_1 = float(argument_list[1])  + 4 # Save distance
                distance_1_semaphore.release()

                # save distance from sensor 2
                distance_2_semaphore.acquire()
                distance_2 = float(argument_list[2]) + 7

                rads = math.radians(degrees)
                x = distance_1 * math.cos(rads)
                y = distance_1 * math.sin(rads)
                scatter_plot_points.append([x, y])

                if ((degrees + 180.0) >360.0):
                    rads = math.radians((degrees+180.0) - 360.0)
                else:
                    rads = math.radians(degrees+180.0)

           
                x = distance_2 * math.cos(rads)
                y = distance_2 * math.sin(rads)
                scatter_plot_points.append([x, y])

                distance_2_semaphore.release()

                #print(str(distance_1) + " " + str(distance_2))

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
    global scatter_plot_points
    global scatter_plot

    plt.gca().set_aspect('equal') 

    # update scatter plot here
    distance_2_semaphore.acquire()
    scatter_plot.set_offsets(scatter_plot_points)
    distance_2_semaphore.release()

    # update sample rate label
    sample_rate_semaphore.acquire()
    sample_rate_label.setText(f"Sample Rate: {sample_rate_disp_val:.2f} (Hz)")
    sample_rate_semaphore.release()

    draw_podium_and_sensors()
    # draw canvas
    canvas.draw()

    
    # run one-shot timer to call update_plot recursively with a 5ms period (200hz canvas refresh rate)
    QTimer.singleShot(25, update_plot)

def draw_podium_and_sensors(): 
    global sensor_1_dot
    global sensor_2_dot
    # Add the circle representing the podium
    podium_circle = plt.Circle((0, 0), 4, color='blue', fill=False, linestyle='dashed')
    plt.gca().add_patch(podium_circle)


    # Update the positions of the red dots based on lidar sensor data
    degrees_semaphore.acquire()
    rads_1 = math.radians(degrees)
    x_1 = 4 * math.cos(rads_1)
    y_1 = 4 * math.sin(rads_1)
    sensor_1_dot.set_data([x_1], [y_1])

    rads_2 = math.radians(degrees + 180.0)
    x_2 = 4 * math.cos(rads_2)
    y_2 = 4 * math.sin(rads_2)
    sensor_2_dot.set_data([x_2], [y_2])
    degrees_semaphore.release()

    # Update the positions of the red dots
    sensor_1_dot.set_data([x_1], [y_1])
    sensor_2_dot.set_data([x_2], [y_2])

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

def start():
   data_stream.write(b'r\n')


def stop():
   data_stream.write(b's\n')

# open comport
data_stream = serial.Serial('COM12', 1000000)

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
window.setGeometry(100, 100, 1600, 1000)

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

# create a start button
start_button = QPushButton("Start")  # button text
start_button.clicked.connect(start)  # connect button to quit() function on click
start_button.setFixedWidth(60)  # set button width
control_layout.addWidget(start_button)  # add button to existing layout

# create a stop button
stop_button = QPushButton("Stop")  # button text
stop_button.clicked.connect(stop)  # connect button to quit() function on click
stop_button.setFixedWidth(60)  # set button width
control_layout.addWidget(stop_button)  # add button to existing layout

# initialize the Matplotlib figure
fig, ax = plt.subplots()
# set the x-axis and y-axis limits and ticks
ax.set_xlim(-50, 50)
ax.set_ylim(-50, 50)
ax.set_xticks(range(-50, 51, 5))
ax.set_yticks(range(-50, 51, 5))
ax.set_xlim(-1000, 1000)
ax.set_ylim(-1000, 1000)
ax.set_xticks(range(-1000, 1001, 100))
ax.set_yticks(range(-1000, 1001, 100))


# create an initial line from (2, 0) to (2, distance_1)
#line_sensor_1, = ax.plot([2, 2], [0, distance_1], color=(0.0, 0.8, 0.0), linewidth=8)
# create an initial line from (8, 0) to (8, distance_2)
#line_sensor_2, = ax.plot([8, 8], [0, distance_2], color=(0.0, 0.8, 0.0), linewidth=8)

scatter_plot = ax.scatter([], [], color=(0.0, 0.8, 0.0), s=5, label='Map Plot')



# add a y-axis label
ax.set_ylabel("Distance (cm)")
ax.set_title("TF-Mini-S Distance Measured")
sensor_1_dot, = plt.plot([], [], 'ro', markersize=5)  # Red dot for sensor 1
sensor_2_dot, = plt.plot([], [], 'ro', markersize=5)  # Red dot for sensor 2
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


scatter_plot_points.append([0,0])
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