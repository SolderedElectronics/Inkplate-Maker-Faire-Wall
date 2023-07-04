# This crops a large image of exact dimensions to display on the wall

# total width of the image needs to be 13170px
# horizontal gaps are 130px

# total height of the image needs to be 5630px
# vertical gaps are 136 px

from PIL import Image
import os


def crop_image_grid(image_path, crop_width, crop_height, offset_x=0, offset_y=0):
    # Open the image file
    img = Image.open(image_path)
    img_width, img_height = img.size

    # Calculate the number of crops in x and y directions
    num_x = (img_width - offset_x) // (crop_width + offset_x)
    num_y = (img_height - offset_y) // (crop_height + offset_y)

    cropped_images = []

    for i in range(num_y+1):
        for j in range(num_x+1):
            # Calculate the position of the current crop
            left = j * (crop_width + offset_x)
            upper = i * (crop_height + offset_y)
            right = left + crop_width
            lower = upper + crop_height

            # Crop the image and append to the list
            cropped_img = img.crop((left, upper, right, lower))
            cropped_images.append(cropped_img)

    return cropped_images


filename = input("Enter the filename: ")
cropped_images = crop_image_grid(filename, 1200, 825, 130, 136)

# Make a folder with the filename's name
parent_directory = os.getcwd()
new_directory = filename[:-4]
path = os.path.join(parent_directory, new_directory)
os.mkdir(new_directory)
os.makedirs(path, exist_ok=True)
print(new_directory)
for i, img in enumerate(cropped_images):
    # Save the cropped images
    img.save(f'{new_directory}/{f"{i:02}"}_{new_directory}.jpg')
