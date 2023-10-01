import os

#pip install --upgrade google-api-python-client google-auth-httplib2 google-auth-oauthlib

from googleapiclient.discovery import build
from google.oauth2 import service_account
from googleapiclient.http import MediaFileUpload


from datetime import datetime
from pathlib import Path

from generate_simulation import CSV_FILE_NAME as CSV_FILE_NAME
from PointPlotting import PLOT_IMAGE_NAME as PLOT_IMAGE_NAME


# Replace with the path to your credentials JSON file
CREDENTIAL_FILE = 'team-phantom-drive-writting-2d92b03341f0.json'

# Define the folder ID of the Google Drive folder where you want to upload the text file
VCU_SIM_LOG_FOLDER_ID = '1wn_EWejA6HYhonjLiEfYkdMhRBiJK7p7'


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
    creds = service_account.Credentials.from_service_account_file(CREDENTIAL_FILE, scopes=['https://www.googleapis.com/auth/drive'])
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
    print(f'Created new parent folder: {folder_name} (Folder ID: {new_folder_id})')

    # Retrieve the ID of the newly created folder
    return new_folder_id


def upload_to_gdrive(service, folder_id):

    now = datetime.now()
    date_time = now.strftime("%Y-%m-%d-%H-%M-%S-")
    
    #write the files as date first so they can be sorted
    folder_name = date_time + "Sim"

    new_folder_id = create_parent_folder(service, folder_name, folder_id)

    upload_csv_to_gdrive(service, CSV_FILE_NAME, new_folder_id)

    upload_png_to_gdrive(service, PLOT_IMAGE_NAME, new_folder_id)

       

def upload_csv_to_gdrive(service, csv_file_path: str, parent_folder_id: str):

    file_stem = Path(csv_file_path).stem
    file_ext = Path(csv_file_path).suffix
    csv_file_name = file_stem + file_ext

    file_metadata = {
        'name': csv_file_name,
        'parents': [parent_folder_id]
        }

    media = MediaFileUpload(csv_file_path, mimetype='text/csv')

    
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


def upload_png_to_gdrive(service, image_file_path: str, parent_folder_id: str):


    file_stem = Path(image_file_path).stem
    file_ext = Path(image_file_path).suffix
    image_file_name = file_stem + file_ext
     # Create a media object for the image file
    image_media = MediaFileUpload(image_file_path, mimetype='image/png')

    # File metadata
    file_metadata = {
        'name': image_file_name,  # Change the file name if needed
        'parents': [parent_folder_id]
    }

    # Upload the image file to Google Drive
    file = service.files().create(
        body=file_metadata,
        media_body=image_media,
        fields='id'
    ).execute()

    print(f'Uploaded image with ID: {file["id"]}')



if __name__ == '__main__':
    service = authenticate_gdrive()

    upload_to_gdrive(service, VCU_SIM_LOG_FOLDER_ID)

