import requests
import json

headers = {"Authorization": "Bearer ya29.a0AfB_byAl4p4lHJVn90vyMudsSneTlEqVp6dgz_M0grLRVGUe3nG07R7XLdBoDeeoZUOrLijvoPgY-yNGQZmGV6WskeEGt8JcwPUGipSTBpCdkH3TZXFoQwzgXev2f9TspYSpekBjm1ViQ_w9YMYZmaziTHdLmbwx69r0aCgYKAUQSARISFQGOcNnClEeC-FM7O9m-lvHPz_W9WA0171"}
para = {
    "name": "image_to_upload.png"
}

files = {
    'data' : ('metadata', json.dumps(para), 'application/json; charset=utf-8'),
    'file' : open("image_to_upload.png", "rb")
}

r = requests.post(
    "https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart",
    headers=headers,
    files=files
)

print(r)

# import os
# from googleapiclient.discovery import build
# from google.oauth2 import service_account

# # Replace with the path to your credentials JSON file
# credentials_file = 'credentials.json'

# # Define the folder ID of the Google Drive folder where you want to upload the image
# folder_id = 'TestWriteFolder'

# def authenticate_gdrive():
#     import pdb; pdb.set_trace()
#     creds = service_account.Credentials.from_service_account_file(credentials_file, scopes=['https://www.googleapis.com/auth/drive'])
#     service = build('drive', 'v3', credentials=creds)
#     return service

# def upload_image_to_gdrive(service, file_path, folder_id):
#     file_name = os.path.basename(file_path)
#     file_metadata = {'name': file_name, 'parents': [folder_id]}
    
#     media = service.files().create(
#         body=file_metadata,
#         media_body=file_path,
#         supportsAllDrives=True
#     ).execute()
    
#     print(f'Uploaded image: {file_name} (File ID: {media["id"]})')

# if __name__ == '__main__':
#     service = authenticate_gdrive()
    
#     # Replace 'image_to_upload.jpg' with the path to the image you want to upload
#     image_to_upload = 'image_to_upload.jpg'
    
#     upload_image_to_gdrive(service, image_to_upload, folder_id)

