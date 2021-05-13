from awscrt import io, mqtt, auth, http
from awsiot import mqtt_connection_builder
import time as t
import json
from datetime import datetime

# Define ENDPOINT, CLIENT_ID, PATH_TO_CERT, PATH_TO_KEY, PATH_TO_ROOT, MESSAGE, TOPIC, and RANGE
ENDPOINT = "a1gznbp4fjkpqr-ats.iot.us-east-1.amazonaws.com"
CLIENT_ID = "seaStation1"
PATH_TO_CERT = "certificates/cert.pem"
PATH_TO_KEY = "certificates/private.key"
PATH_TO_ROOT = "certificates/rootCA.pem"
TOPIC = "data/seastation/1"
RANGE = 20

def mqtt_publish(lat,long,speed,floater_id,file_id,counter):
    # Spin up resources
    event_loop_group = io.EventLoopGroup(1)
    host_resolver = io.DefaultHostResolver(event_loop_group)
    client_bootstrap = io.ClientBootstrap(event_loop_group, host_resolver)
    mqtt_connection = mqtt_connection_builder.mtls_from_path(
                endpoint=ENDPOINT,
                cert_filepath=PATH_TO_CERT,
                pri_key_filepath=PATH_TO_KEY,
                client_bootstrap=client_bootstrap,
                ca_filepath=PATH_TO_ROOT,
                client_id=CLIENT_ID,
                clean_session=False,
                keep_alive_secs=6
                )
    print("Connecting to {} with client ID '{}'...".format(
            ENDPOINT, CLIENT_ID))
    # Make the connect() call
    connect_future = mqtt_connection.connect()
    # Future.result() waits until a result is available
    connect_future.result()
    print("Connected!")
    # Publish message to server desired number of times.
    print('Begin Publish')
        
    message = {"lat" : lat,"long": long,"station_id": floater_id, "url": file_id,"violation_id":counter}
    mqtt_connection.publish(topic=TOPIC, payload=json.dumps(message), qos=mqtt.QoS.AT_LEAST_ONCE)
    print("Published: '" + json.dumps(message) + "' to the topic: " + TOPIC)
    t.sleep(0.1)
    print('Publish End')
    disconnect_future = mqtt_connection.disconnect()
    disconnect_future.result()
