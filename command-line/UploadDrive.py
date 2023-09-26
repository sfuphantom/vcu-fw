import os
from googleapiclient.discovery import build
from google.oauth2 import service_account
from googleapiclient.http import MediaFileUpload


from datetime import datetime
from pathlib import Path


# Replace with the path to your credentials JSON file
credentials_file = 'team-phantom-drive-writting-2d92b03341f0.json'

# Define the folder ID of the Google Drive folder where you want to upload the text file
folder_id = 'Myfolder'


def share_file_with_user(service, file_id, user_email, role='reader'):
    permission = {
        'type': 'user',
        'role': role,
        'emailAddress': user_email
    }
    service.permissions().create(
        fileId=file_id,
        body=permission,
        sendNotificationEmail=False
    ).execute()

def authenticate_gdrive():
    creds = service_account.Credentials.from_service_account_file(credentials_file, scopes=['https://www.googleapis.com/auth/drive'])
    service = build('drive', 'v3', credentials=creds)
    return service

def upload_csv_to_gdrive(service, file_path, folder_id):

    file_stem = Path(file_path).stem
    file_ext = Path(file_path).suffix
    now = datetime.now()
    date_time = now.strftime("%Y-%m-%d-%H-%M-%S-")
    
    #write the files as date first so they can be sorted
    file_name = date_time + file_stem + file_ext
    

    #parents is the folder Id for the following team drive directory: 
    """
    Drive/Phantom Engineering/Controls/2023/Test Logs-Simulation/VCU
    """
    file_metadata = {
        'name': file_name,
        'parents': ['1Ov78ECHD-shFPP5gsG9A1I7Q_OoVcg7-']
        }

    media = MediaFileUpload(file_path, mimetype='text/csv')

    
    media = service.files().create(
        body=file_metadata,
        media_body=media,
        supportsAllDrives=True
    ).execute()
    
    print(f'Uploaded CSV file: {file_name} (File ID: {media["id"]})')

    #share with the logged in user/team phantom drive.
    # user_email = 'klitvin101@gmail.com'  # Replace with your personal Google account email
    # share_file_with_user(service, media["id"], user_email)

    

if __name__ == '__main__':
    service = authenticate_gdrive()
    
    # Replace 'data.csv' with the path to the CSV file you want to upload in the relative directory
    csv_file_to_upload = "SimulatedValues.csv" # current date and time for file uniqueness
    
    upload_csv_to_gdrive(service, csv_file_to_upload, folder_id)



#--------------Unused---------------#
#->access tokens refresh every 3600 seconds, and we would need to write a script to query 
#when a tokens get's refreshed
# import requests
# import json

# headers = {"Authorization": "Bearer ya29.a0AfB_byAl4p4lHJVn90vyMudsSneTlEqVp6dgz_M0grLRVGUe3nG07R7XLdBoDeeoZUOrLijvoPgY-yNGQZmGV6WskeEGt8JcwPUGipSTBpCdkH3TZXFoQwzgXev2f9TspYSpekBjm1ViQ_w9YMYZmaziTHdLmbwx69r0aCgYKAUQSARISFQGOcNnClEeC-FM7O9m-lvHPz_W9WA0171"}
# para = {
#     "name": "TestVals/SimValsLOL.csv",
#     "parents" : "Myfolder/"
# }

# files = {
#     'data' : ('metadata', json.dumps(para), 'application/json; charset=utf-8'),
#     'file' : open("SimulatedValues.csv", "rb")
# }

# r = requests.post(
#     "https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart",
#     headers=headers,
#     files=files
# )

# print(r)
