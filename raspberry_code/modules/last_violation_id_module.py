import boto3


def get_violation_id(dynamodb=None):

    # Create dynamo connection
    if not dynamodb:
        dynamodb = boto3.resource('dynamodb',region_name='us-east-1')

    # Choose Table
    table = dynamodb.Table('violation')
   
    
    # Pass parameters
    scan_kwargs = {
       
        'ProjectionExpression': "device_data.violation_id"
    }


    max_id = -1
    done = False
    start_key = None

    # Update max
    while not done:
        if start_key:
            scan_kwargs['ExclusiveStartKey'] = start_key
        response = table.scan(**scan_kwargs)
        ids = response.get('Items', [])

        for id in ids:
            curr = int(id['device_data']['violation_id'])
            if curr > max_id:
                max_id = curr
        
        start_key = response.get('LastEvaluatedKey', None)
        done = start_key is None
    
    print(max_id)
    return max_id+1

def run_legacy():
    import subprocess
    subprocess = subprocess.Popen("/usr/local/opt/python-3.9.5/bin/python3.9 -c 'from modules import last_violation_id_module;last_violation_id_module. get_violation_id()'",
                                  shell=True, stdout=subprocess.PIPE)
    subprocess_return = subprocess.stdout.read()
    
    subprocess.wait()
    
    result = int(subprocess_return.decode("utf-8").strip())
    print(result)
    
    return result+1
    