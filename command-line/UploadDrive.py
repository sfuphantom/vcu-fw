import os

#pip install --upgrade google-api-python-client google-auth-httplib2 google-auth-oauthlib

from googleapiclient.discovery import build, Resource
from google.oauth2 import service_account
from google.oauth2.service_account import Credentials
from googleapiclient.http import MediaFileUpload


from datetime import datetime
from pathlib import Path

from data_generation import CSV_FILE_NAME, PLOT_IMAGE_NAME



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

    def __init__(self, drive_service : build , folder_id : str):
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

        self.set_read_only_permissions(file_id = media['id'])

        print(f'Uploaded CSV file: {csv_file_name} (File ID: {media["id"]})')

        return True
    
    def set_read_only_permissions(self, file_id: str):
         # Set the file as read-only (Viewer)
        self.drive_service.permissions().create(
            fileId=file_id,
            body={'role': 'reader', 'type': 'anyone'}
        ).execute()

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
    
class VcuGDriveInterface:
    """
    Specific class to interact with the VCU logging folder within Team Phantom Google Drive
    """

    #DO NOT POST THIS ON THE GITHUB OPEN SOURCE REPO
    CREDENTIAL_FILE = 'team-phantom-drive-writting-2d92b03341f0.json'

    #parent folder is the folder Id for the following team drive directory: 

    #---Drive/Phantom Engineering/Controls/2023/Test Logs-Simulation/VCU---
    VCU_SIM_LOG_FOLDER_ID = '1wn_EWejA6HYhonjLiEfYkdMhRBiJK7p7'

    def __init__(self) -> None:
        Authenticator = GoogleDriveAuthenticator(self.CREDENTIAL_FILE)
        credentials = Authenticator.generate_credentials()
        gdrive_service = Authenticator.authenticate_gdrive(credentials=credentials)
        self.upload_gdrive = GooleDriveUpload(gdrive_service, self.VCU_SIM_LOG_FOLDER_ID)

    def upload_data(self, csv_file_name: str = CSV_FILE_NAME, plotted_data:str = PLOT_IMAGE_NAME):
        #write the files as date first so they can be sorted
        now = datetime.now()
        date_time = now.strftime("%Y-%m-%d-%H-%M-%S-") 
        folder_name = date_time + "Sim"

        #create the parent folder to store the CSV and data
        try: 
            parentID: str = self.upload_gdrive.create_parent_folder(
                                                folder_name=folder_name, 
                                                parent_folder_id=self.VCU_SIM_LOG_FOLDER_ID)
            #upload the csv
            self.upload_gdrive.upload_csv_to_gdrive(
                                CSV_FILE_NAME, parentID)
            #upload the png
            self.upload_gdrive.upload_png_to_gdrive(
                                PLOT_IMAGE_NAME, parentID)
        except Exception as e:
            print(f"Exception Caught: {e}")
            return False
        
        return True

    

