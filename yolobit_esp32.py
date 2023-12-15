# Import necessary libraries and modules
from event_manager import *
from machine import Pin, SoftI2C
from aiot_dht20 import DHT20
from yolobit import *
# Disable button actions and assignments
button_a.on_pressed = None
button_b.on_pressed = None
button_a.on_pressed_ab = button_b.on_pressed_ab = -1

# Initialize variables and clear event manager
event_manager.reset()

# Initialize DHT20 sensor using SoftI2C
aiot_dht20 = DHT20(SoftI2C(scl=Pin(22), sda=Pin(21)))

# Initialize LCD1602 display
aiot_lcd1602 = LCD1602()

# Callback function for successful HTTP POST response
def on_http_response_successed_callback_post():
    print('POST', end=' ')

# Callback function for failed HTTP response
def on_http_response_failed_callback():
    pass

# Callback function for successful HTTP GET response
def on_http_response_successed_callback_get():
    # Global variables for data and statuses
    global RT, Status, data, headers, RH, SM, LUX, list2, Fan_Intensity, Pump_Switch, http_response
    list2 = http_response.text
    Fan_Intensity = ujson.loads(list2)['yolo-fan']
    Pump_Switch = ujson.loads(list2)['yolo-servo']
    pin14.write_analog(round(translate(Fan_Intensity, 0, 100, 0, 1023)))
    print('GET', end=' ')

# Callback function for periodic event to gather sensor data, display on LCD, and publish via MQTT and HTTP POST
def on_event_timer_callback_G_p_m_h_O():
    # Global variables for data and statuses
    global RT, Status, data, headers, RH, SM, LUX, list2, Fan_Intensity, Pump_Switch, http_response
    # Read sensor data
    aiot_dht20.read_dht20()
    RT = aiot_dht20.dht20_temperature()
    RH = aiot_dht20.dht20_humidity()
    SM = round(translate((pin1.read_analog()), 0, 4095, 0, 100))
    LUX = round(translate((pin2.read_analog()), 0, 4095, 0, 100))
    # Prepare data to be sent
    data = {
        "overview.dht20-humidity": RH,
        "overview.dht20-temperature": RT,
        "overview.soil-moisture": SM,
        "overview.light": LUX
    }
    headers = {
        "Content-Type": 'application/json'
    }
    # Display sensor data on LCD
    aiot_lcd1602.move_to(0, 0)
    aiot_lcd1602.putstr('RT:')
    # ... (LCD display instructions)

    # Publish data via MQTT
    mqtt.publish('V1', 'Hello')
    mqtt.publish('V2', RH)
    mqtt.publish('V3', SM)
    mqtt.publish('V4', LUX)

    # Perform HTTP POST with collected data
    http_response = urequests.post('http://192.168.4.1/postdata', data=None, json=data, headers=headers)
    http_response.on_successed(on_http_response_successed_callback_post)
    http_response.on_failed(on_http_response_failed_callback)

# Add timer event to execute data gathering and transmission periodically
event_manager.add_timer_event(30000, on_event_timer_callback_G_p_m_h_O)

# Callback function for periodic event to perform HTTP GET request
def on_event_timer_callback_N_s_t_P_s():
    # Global variables for data and statuses
    global RT, Status, data, headers, RH, SM, LUX, list2, Fan_Intensity, Pump_Switch, http_response
    # Perform HTTP GET request
    http_response = urequests.get('http://192.168.4.1/getData')
    http_response.on_successed(on_http_response_successed_callback_get)
    http_response.on_failed(on_http_response_failed_callback)

# Add timer event to execute HTTP GET request periodically
event_manager.add_timer_event(1000, on_event_timer_callback_N_s_t_P_s)

# Connect to Wi-Fi and set Status to 1
if True:
    mqtt.connect_wifi('ESP32_AP', '123456789')
    Status = 1

# Run the event manager
while True:
    event_manager.run()
