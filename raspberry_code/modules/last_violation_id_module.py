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


    max_id = 0
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
    
    return max_id+1