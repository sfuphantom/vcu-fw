#Author : Kevin Litvin
#Date : November 2023
#Description : Authentication and communication with the Team Phantom Google Drive for cloud data storage and result logging

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
    """
    Class to encapsulate google drive authentication for the Google Cloud Platform
    
    This class offers functionality for the authentication of 
    a specific Google Cloudd Platform API dependant on the scope, service, and JSON creds provided
    """

    def __init__(self, credentials_path : str):
        self.credentials_path : str = credentials_path

    def generate_credentials(self, scope: list[str] = ['https://www.googleapis.com/auth/drive']) -> Credentials:
        """
        Generate the necessary credentials given the scope of the Google Drive API

        :param scope: list of the of scope which the credentials are generate. Drive scope default to gdrive if not specific
        
        :return: Credentials object after building using credential file:
        
        :raises FileNotFoundError: If the JSON file with the required Google Drive credentials is not found in the expected location.
        """
        creds : Credentials = service_account.Credentials.from_service_account_file(self.credentials_path, scopes=scope)
        return creds

    def authenticate_gdrive(self, credentials: service_account.Credentials, service: str = 'drive', version: str = 'v3') -> Resource:
        """
        Build the service to interact with Google Cloud Platform API

        :param credentials: credentials object with contianing credentials for the specific google drive authentication
        :param server: type of service to authenticate, default to drvie if not specified
        :param version: version of the the authentication, default to version 3 (latest) if not specified

        :return: resource object used to authenticate accesss 
        """
        service : Resource = build(serviceName=service, version=version, credentials=credentials)
        return service
    

class GoogleDriveUpload:

    def __init__(self, drive_service : build , folder_id : str):
        self.drive_service : Resource= drive_service
        self.folder_id: str = folder_id
    

    def create_parent_folder(self, folder_name: str, parent_folder_id: str = None) -> str:
        """
        Create a parent folder to hold any subdirectories or files with a specific drive ID
        If not parent_folder is generated, use the curent parent folder associated with the 
        the objects constrution

        :param folder_name: name of the folder to be generated
        :param parent_folder_id: the folder ID where the this folder will reside in
        
        :return: the ID for the the generated folder
        """
        if parent_folder_id is None:
            parent_folder_id = self.folder_id

        # Create the new folder information and specific that it's a folder
        folder_metadata = {
            'name': folder_name,
            'parents': [parent_folder_id],
            'mimeType': 'application/vnd.google-apps.folder'  # Indicates that it's a folder
        }

        #create the folder data and upload it to the parent ID location
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

        :param csv_file_path: relative path to the csv file to be uploaded
        :param parent_folder_id: ID of the folder which this file is uploaded to 
        
        :return: True if succesfully uplaoded
        """

        if parent_folder_id is None:
            parent_folder_id = self.folder_id

        csv_file_name = self.path_to_file_name(csv_file_path)

        #set the file metadata
        file_metadata = {
            'name': csv_file_name,
            'parents': [parent_folder_id]
         }

        #Create an uploadable csv object
        media = MediaFileUpload(csv_file_path, mimetype='text/csv')

        media = self.drive_service.files().create(
            body=file_metadata,
            media_body=media,
            supportsAllDrives=True
        ).execute()

        self.set_read_only_permissions(file_id = media['id'])

        print(f'Uploaded CSV file: {csv_file_name} (File ID: {media["id"]})')

        return True
    
    def set_read_only_permissions(self, file_id: str) -> bool:
        """
        Set the permission of a file to read only

        :param file_id: ID of the file to restrict permissions
        :return: True if succesful, false otherwise
        """
         # Set the file as read-only (Viewer)
        self.drive_service.permissions().create(
            fileId=file_id,
            body={'role': 'reader', 'type': 'anyone'}
        ).execute()

        return True

    def upload_png_to_gdrive(self, image_file_path: str, parent_folder_id: str) -> bool:
        """
        Uploads a file of type png extension to a spefific folder on the drive

        :param image_file_path: relativ path of the image to uplaoded
        :param parent_folder_id: ID of the folder which will contain image

        :return: True if succesfully uploaded 
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
    def path_to_file_name(cls, file_path: str) -> str:
        """
        Used to modify the file name to fit the correct extension
        """
        file_stem = Path(file_path).stem
        file_ext = Path(file_path).suffix
        return file_stem + file_ext
    
class VcuGDriveInterface:
    """
    Builds and authenticates write services for VCU logging folder within Team Phantom Google Drive

    :Attributs: Contains the data for the credential file and the folder ID for the logging folder
    """

    #DO NOT POST THIS ON THE GITHUB OPEN SOURCE REPO
    CREDENTIAL_FILE = 'team-phantom-drive-writting-2d92b03341f0.json'

    #parent folder is the folder Id for the following team drive directory: 

    #---Drive/Phantom Engineering/Controls/2023/Test Logs-Simulation/VCU---
    VCU_SIM_LOG_FOLDER_ID = '1wn_EWejA6HYhonjLiEfYkdMhRBiJK7p7'

    def __init__(self):
        Authenticator = GoogleDriveAuthenticator(self.CREDENTIAL_FILE)
        credentials = Authenticator.generate_credentials()
        gdrive_service = Authenticator.authenticate_gdrive(credentials=credentials)
        self.upload_gdrive : GoogleDriveUpload = GoogleDriveUpload(gdrive_service, self.VCU_SIM_LOG_FOLDER_ID)

    def upload_data(self, csv_file_name: str = CSV_FILE_NAME, png_file_name:str = PLOT_IMAGE_NAME) -> bool:
        """
        Generates a new folder conatainer for the simulated data results. Folder is generated 
        based on the current time of the upload request.

        :param csv_file_name: relative path to the csv file 
        :param plotted_data: relative path to png file

        :return: True if succesfully uplaoded, False otherwise
        """
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
                                csv_file_name, parentID)
            #upload the png
            self.upload_gdrive.upload_png_to_gdrive(
                                png_file_name, parentID)
        except Exception as e:
            print(f"Exception Caught: {e}")
            return False
        
        return True

    

