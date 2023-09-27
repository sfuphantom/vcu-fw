import os
from googleapiclient.discovery import build
from google.oauth2 import service_account
from googleapiclient.http import MediaFileUpload


from datetime import datetime
from pathlib import Path


# Replace with the path to your credentials JSON file
credentials_file = 'team-phantom-drive-writting-2d92b03341f0.json'

# Define the folder ID of the Google Drive folder where you want to upload the text file
folder_id = '1Ov78ECHD-shFPP5gsG9A1I7Q_OoVcg7-'


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

def create_parent_folder(service, folder_name: str, parent_folder_id: str):

    """
    Create folder to store CSV file and matplot lib file
    """
    
    
    #parent folder is the folder Id for the following team drive directory: 
    """
    Drive/Phantom Engineering/Controls/2023/Test Logs-Simulation/VCU
    """

    # Create the new folder
    folder_metadata = {
        'name': folder_name,
        'parents': [parent_folder_id],
        'mimeType': 'application/vnd.google-apps.folder'  # Indicates that it's a folder
    }

    new_folder = service.files().create(
        body=folder_metadata,
        fields='id'
    ).execute()

    new_folder_id: str = new_folder.get('id')
    print(f'Created Folder: {folder_name} (File ID: {folder_id})')

    # Retrieve the ID of the newly created folder
    return new_folder_id


def upload_csv_to_gdrive(service, file_path, folder_id):

    file_stem = Path(file_path).stem
    file_ext = Path(file_path).suffix
    now = datetime.now()
    date_time = now.strftime("%Y-%m-%d-%H-%M-%S-")
    
    #write the files as date first so they can be sorted
    folder_name = date_time + "Sim"
    csv_file_name = file_stem + file_ext

    new_folder_id = create_parent_folder(service, folder_name, folder_id)

    file_metadata = {
        'name': csv_file_name,
        'parents': [new_folder_id]
        }

    media = MediaFileUpload(file_path, mimetype='text/csv')

    
    media = service.files().create(
        body=file_metadata,
        media_body=media,
        supportsAllDrives=True
    ).execute()

     # Set the file as read-only (Viewer)
    service.permissions().create(
        fileId=media['id'],
        body={'role': 'reader', 'type': 'anyone'}
    ).execute()


    print(f'Uploaded CSV file: {csv_file_name} (File ID: {media["id"]})')


        # Create a media object for the image file
    image_media = MediaFileUpload('multiple_datasets_plot.png', mimetype='image/png')

    # File metadata
    file_metadata = {
        'name': 'Simulation Visualized.png',  # Change the file name if needed
        'parents': [new_folder_id]
    }

    # Upload the image file to Google Drive
    file = service.files().create(
        body=file_metadata,
        media_body=image_media,
        fields='id'
    ).execute()

    print(f'Uploaded image with ID: {file["id"]}')
    # Send a request to the Drive API to retrieve the sharing information for the folder
    # response = service.files().get(fileId=folder_id, fields='permissions').execute()

    # # Extract and print the email addresses of users and groups with access
    # permissions = response.get('permissions', [])
    # for permission in permissions:
    #     email_address = permission.get('emailAddress')
    #     if email_address:
    #         print(f"Email: {email_address}")
    

    # Authorize the user and get an access token
    # authorization_url, _ = credentials.authorization_url('https://accounts.google.com/o/oauth2/auth')
    # print("Please visit this URL to authorize access:", authorization_url)
    # authorization_code = input("Enter the authorization code: ")

    # credentials.fetch_token(
    #     'https://accounts.google.com/o/oauth2/token',
    #     authorization_response=authorization_code
    # )

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
