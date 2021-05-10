from __future__ import print_function
import os.path
from googleapiclient.discovery import build
from googleapiclient.http import MediaFileUpload
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials

# If modifying these scopes, delete the file token.json.
SCOPES = ['https://www.googleapis.com/auth/drive']

def drive_upload(photo_name="pic.jpg"):
    """Shows basic usage of the Drive v3 API.
    Prints the names and ids of the first 10 files the user has access to.
    """
    creds = None
    # The file token.json stores the user's access and refresh tokens, and is
    # created automatically when the authorization flow completes for the first
    # time.
    try:
        if os.path.exists('token.json'):
            creds = Credentials.from_authorized_user_file('token.json', SCOPES)
        # If there are no (valid) credentials available, let the user log in.
        if not creds or not creds.valid:
            if creds and creds.expired and creds.refresh_token:
                creds.refresh(Request())
            else:
                flow = InstalledAppFlow.from_client_secrets_file(
                    'credentials.json', SCOPES)
                creds = flow.run_local_server(port=0)
            # Save the credentials for the next run
            with open('token.json', 'w') as token:
                token.write(creds.to_json())

        service = build('drive', 'v3', credentials=creds)

        # Call the Drive v3 API
        print("Begin Drive upload of "+ photo_name)
        file_metadata = {'name': photo_name,'parents':["1euot5RlQOXgl7bV8yZWktXFMeZJ8HsuG"]}
        media = MediaFileUpload(photo_name, mimetype='image/jpeg',resumable=True)
        file = service.files().create(body=file_metadata,
                                        media_body=media,
                                        fields='id').execute()

        print("Upload Complete!")
        
        file_id = file.get('id')
        print("Upload of "+ photo_name+" succeded!")
        print('File ID: %s' % file_id)
        print("Setting permssions...")
        
        def callback(request_id, response, exception):
            if exception:
                # Handle error
                print(exception)
                return -1
            else:
                print("Permission Id: %s" % response.get('id'))
                print("Drive upload fully succeced!")
        # Change permissions to anyone with the link can see the photo
        batch = service.new_batch_http_request(callback=callback)
        user_permission = {
            'type': 'anyone',
            'role': 'reader',
        }
        batch.add(service.permissions().create(
                fileId=file_id,
                body=user_permission,
                fields='id',
        ))
        batch.execute()
        return file_id
    
    except Exception as e:
        print("Error occurred!")
        print(e)
        return -1
