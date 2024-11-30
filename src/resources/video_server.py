import cv2
import numpy as np
import os
import time

def get_luminance(img):
    """
    Compute the relative luminance of an image using the specified formula.

    Args:
        img (numpy.ndarray): Input RGB image (H, W, C) normalized to [0, 1].

    Returns:
        numpy.ndarray: Grayscale luminance image (H, W).
    """
    img = np.swapaxes(img, 0, 2)  

    img = np.power(img, 2.2)

    bw_img = 0.2126 * img[0] + 0.7152 * img[1] + 0.0722 * img[2]
    return bw_img

def camera_to_2d_texture(output_directory, texture_size=(128, 128), fps=60):
    """
    Captures frames from the camera, processes them, and saves them as binary 2D textures.

    Args:
        output_directory (str): Directory to save the output binary files.
        texture_size (tuple): Desired size of the texture (height, width). Default is (128, 128).
        fps (int): Desired frames per second for processing. Default is 60.
    """
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        raise IOError("Error: Unable to access the camera.")

    target_frame_time = 1.0 / fps 

    os.makedirs(output_directory, exist_ok=True)

    frame_count = 0

    try:
        while True:
            start_time = time.time()

            ret, frame = cap.read()
            if not ret:
                print("Error: Unable to read frame from the camera.")
                break

            resized_frame = cv2.resize(frame, texture_size)

            normalized_frame = resized_frame.astype(np.float32) / 255.0

            luminance_frame = get_luminance(normalized_frame)

            luminance_frame = luminance_frame.astype(np.float16)

            output_file = os.path.join(output_directory, f"video.raw")
            with open(output_file, "wb") as file:
                file.write(luminance_frame.tobytes(order='F'))

            print(f"Frame {frame_count} saved as {output_file}.")
            frame_count += 1

            elapsed_time = time.time() - start_time
            sleep_time = max(0, target_frame_time - elapsed_time)
            time.sleep(sleep_time)

    finally:
        cap.release()
        print(f"Finished capturing {frame_count} frames.")

output_directory = "camera_frames"  
camera_to_2d_texture(output_directory, fps=60)
