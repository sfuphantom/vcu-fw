import os

#pip install --upgrade google-api-python-client google-auth-httplib2 google-auth-oauthlib

from googleapiclient.discovery import build, Resource
from google.oauth2 import service_account
from google.oauth2.service_account import Credentials
from googleapiclient.http import MediaFileUpload


from datetime import datetime
from pathlib import Path

from generate_simulation import CSV_FILE_NAME as CSV_FILE_NAME
from PointPlotting import PLOT_IMAGE_NAME as PLOT_IMAGE_NAME


# Replace with the path to your credentials JSON file
CREDENTIAL_FILE = 'team-phantom-drive-writting-2d92b03341f0.json'

# Define the folder ID of the Google Drive folder where you want to upload the text file
VCU_SIM_LOG_FOLDER_ID = '1wn_EWejA6HYhonjLiEfYkdMhRBiJK7p7'

class GoogleDriveAuthenticator:

    def __init__(self, credentials_path : str):
        self.credentials_path : str = credentials_path

    def generate_credentials(self, scope: list[str] = ['https://www.googleapis.com/auth/drive']) -> Credentials:
        """
        Generate the necessary credentials given the scope of the Google Drive API
        """
        creds : Credentials = service_account.Credentials.from_service_account_file(self.credentials_path, scopes=scope)
        return creds

    def authenticate_gdrive(self, credentials: service_account.Credentials, service: str = 'drive', version: str = 'v3') -> Resource:
        """
        Build the service to interact with Google Cloud Platform API
        """
        service : Resource = build(serviceName=service, version=version, credentials=credentials)
        return service
    

class GooleDriveUpload:

    def __init__(self, drive_service : build , folder_id : str = VCU_SIM_LOG_FOLDER_ID):
        self.drive_service : Resource= drive_service
        self.folder_id: str = folder_id
    

    def create_parent_folder(self, folder_name: str, parent_folder_id: str = None) -> str:

        """
        Create folder to store CSV file and matplot lib file
        """
        if parent_folder_id is None:
            parent_folder_id = self.folder_id

        # Create the new folder
        folder_metadata = {
            'name': folder_name,
            'parents': [parent_folder_id],
            'mimeType': 'application/vnd.google-apps.folder'  # Indicates that it's a folder
        }

        new_folder = self.drive_service.files().create(
            body=folder_metadata,
            fields='id'
        ).execute()

        new_folder_id: str = new_folder.get('id')
        print(f'Created new parent folder: {folder_name} (Folder ID: {new_folder_id})')

        # Retrieve the ID of the newly created folder
        return new_folder_id    

    def upload_csv_to_gdrive(self, csv_file_path: str, parent_folder_id: str = None) -> bool:
        """
        Uploads a file of type csv to to the parent folder given
        """

        if parent_folder_id is None:
            parent_folder_id = self.folder_id

        csv_file_name = self.path_to_file_name(csv_file_path)

        file_metadata = {
            'name': csv_file_name,
            'parents': [parent_folder_id]
            }

        media = MediaFileUpload(csv_file_path, mimetype='text/csv')

        media = self.drive_service.files().create(
            body=file_metadata,
            media_body=media,
            supportsAllDrives=True
        ).execute()

        # Set the file as read-only (Viewer)
        self.drive_service.permissions().create(
            fileId=media['id'],
            body={'role': 'reader', 'type': 'anyone'}
        ).execute()

        print(f'Uploaded CSV file: {csv_file_name} (File ID: {media["id"]})')

        return True


    def upload_png_to_gdrive(self, image_file_path: str, parent_folder_id: str) -> bool:

        """
        Uploads a file of type png extension
        """

        image_file_name = self.path_to_file_name(image_file_path)
        # Create a media object for the image file
        image_media = MediaFileUpload(image_file_path, mimetype='image/png')

        # File metadata
        file_metadata = {
            'name': image_file_name,  # Change the file name if needed
            'parents': [parent_folder_id]
        }

        # Upload the image file to Google Drive
        file = self.drive_service.files().create(
            body=file_metadata,
            media_body=image_media,
            fields='id'
        ).execute()

        print(f'Uploaded image with ID: {file["id"]}')

        return True

    @classmethod
    def path_to_file_name(cls, file_path: str):
        file_stem = Path(file_path).stem
        file_ext = Path(file_path).suffix
        return file_stem + file_ext

if __name__ == '__main__':
    Authenticator = GoogleDriveAuthenticator(credentials_path=CREDENTIAL_FILE)
    credentials = Authenticator.generate_credentials()
    gdrive_service = Authenticator.authenticate_gdrive(credentials=credentials)

    #parent folder is the folder Id for the following team drive directory: 
    """
    Drive/Phantom Engineering/Controls/2023/Test Logs-Simulation/VCU
    """

    UploadGDrive = GooleDriveUpload(gdrive_service)

    now = datetime.now()
    date_time = now.strftime("%Y-%m-%d-%H-%M-%S-")
    
    #write the files as date first so they can be sorted
    folder_name = date_time + "Sim"
    parentID: str = UploadGDrive.create_parent_folder(folder_name=folder_name, parent_folder_id=VCU_SIM_LOG_FOLDER_ID)
    UploadGDrive.upload_csv_to_gdrive(CSV_FILE_NAME, parentID)
    UploadGDrive.upload_png_to_gdrive(PLOT_IMAGE_NAME, parentID)

