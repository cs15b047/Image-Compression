from codec import *
from util import *
import zlib
import pickle

def quantize(latents):
  quantized_latents = (latents / (255 * 0.18215) + 0.5).clamp(0,1)
  quantized = quantized_latents.cpu().permute(0, 2, 3, 1).detach().numpy()[0]
  quantized = (quantized * 255.0 + 0.5).astype(np.uint8)
  return quantized

def unquantize(quantized):
  unquantized = quantized.astype(np.float32) / 255.0
  unquantized = unquantized[None].transpose(0, 3, 1, 2)
  unquantized_latents = (unquantized - 0.5) * (255 * 0.18215)
  unquantized_latents = torch.from_numpy(unquantized_latents)
  return unquantized_latents.to(torch_device)

@torch.no_grad()
def denoise(latents):  
    latents = latents * 0.18215
    step_size = 15
    num_inference_steps = scheduler.config.get("num_train_timesteps", 1000) // step_size
    strength = 0.04
    scheduler.set_timesteps(num_inference_steps)
    offset = scheduler.config.get("steps_offset", 0)
    init_timestep = int(num_inference_steps * strength) + offset
    init_timestep = min(init_timestep, num_inference_steps)
    timesteps = scheduler.timesteps[-init_timestep]
    timesteps = torch.tensor([timesteps], dtype=torch.long, device=torch_device)
    extra_step_kwargs = {}
    if "eta" in set(inspect.signature(scheduler.step).parameters.keys()):
        extra_step_kwargs["eta"] = 0.9
    latents = latents.to(unet.dtype).to(torch_device)
    t_start = max(num_inference_steps - init_timestep + offset, 0)
    with autocast():
        for i, t in enumerate(scheduler.timesteps[t_start:]):
            noise_pred = unet(latents, t, encoder_hidden_states=uncond_embeddings).sample
            latents = scheduler.step(noise_pred, t, latents, **extra_step_kwargs).prev_sample
    #reset scheduler to free cached noise predictions
    scheduler.set_timesteps(1)
    
    return latents / 0.18215

def save_compressed_image(latents, compressed_path):
    latents_compressed = zlib.compress(latents.numpy().tobytes())
    latents_shape = latents.shape
    compressed_data = [latents_compressed, latents_shape]
    with open(compressed_path, 'wb') as f:
        pickle.dump(compressed_data, f)

def restore_latents(compressed_path):
    with open(compressed_path, 'rb') as f:
        compressed_data = pickle.load(f)
    latents_compressed, latents_shape = compressed_data
    latents_compressed = zlib.decompress(latents_compressed)
    latents = np.frombuffer(latents_compressed, dtype=np.float32).reshape(latents_shape)
    latents = torch.from_numpy(latents)
    return latents

def compress_input(input_filepath, output_path):
    gt_img, original_size = resize_to_512(input_filepath)
    gt_original_size = gt_img.resize(original_size, Image.LANCZOS)
    print('Start Encoding...')
    # Display VAE roundtrip image
    latents = to_latents(gt_img)
    print(latents.shape, latents.dtype)
    compressed_path = output_path + '.z'
    save_compressed_image(latents, compressed_path)
    print("Encoding image to latents done. Start decoding")
    latents_restored = restore_latents(compressed_path)

    img_from_latents = to_img(latents_restored)
    img_from_latents = img_from_latents.resize(original_size)
    img_from_latents.save(input_filepath + '_from_latents.png')
    img_from_latents.show()
    print('Decoding complete')
    print('VAE roundtrip')
    print_metrics(gt_original_size, img_from_latents)

    # Quantize latent representation and save as lossless webp image
    print ('Start Quantization...')
    quantized = quantize(latents)
    del latents
    quantized_img = Image.fromarray(quantized)
    quantized_img.save(output_path + "_sd_quantized_latents.webp", lossless=True, quality=100)
    print('Quantization complete')

    # # Display VAE decoded image from 8-bit quantized latents
    # unquantized_latents = unquantize(quantized)
    # unquantized_img = to_img(unquantized_latents)
    # unquantized_img.show()
    # del unquantized_latents
    # print('VAE decoded from 8-bit quantized latents')
    # print_metrics(gt_img, unquantized_img)

    # # further quantize to palette. Use libimagequant for Dithering
    # attr = liq.Attr()
    # attr.speed = 1
    # attr.max_colors = 256
    # input_image = attr.create_rgba(quantized.flatten('C').tobytes(),
    #                                 quantized_img.width,
    #                                 quantized_img.height,
    #                                 0)
    # quantization_result = input_image.quantize(attr)
    # quantization_result.dithering_level = 1.0
    # # Get the quantization result
    # out_pixels = quantization_result.remap_image(input_image)
    # out_palette = quantization_result.get_palette()
    # np_indices = np.frombuffer(out_pixels, np.uint8)
    # np_palette = np.array([c for color in out_palette for c in color], dtype=np.uint8)

    # sd_palettized_bytes = io.BytesIO()
    # np.savez_compressed(sd_palettized_bytes, w=64, h=64, i=np_indices.flatten(), p=np_palette)
    # with open(output_path + ".npz", "wb") as f:
    #     f.write(sd_palettized_bytes.getbuffer())

    # # Compress the dithered 8-bit latents using zlib and save them to disk
    # compressed_bytes = zlib.compress(
    #     np.concatenate((np_palette, np_indices), dtype=np.uint8).tobytes(),
    #     level=9
    #     )
    # with open(output_path + ".bin", "wb") as f:
    #     f.write(compressed_bytes)
    # sd_bytes = len(compressed_bytes)

    # # Display VAE decoding of dithered 8-bit latents
    # np_indices = np_indices.reshape((64,64))
    # palettized_latent_img = Image.fromarray(np_indices, mode='P')
    # palettized_latent_img.putpalette(np_palette, rawmode='RGBA')
    # latents = np.array(palettized_latent_img.convert('RGBA'))
    # latents = unquantize(latents)
    # palettized_img = to_img(latents)
    # palettized_img.show()
    # print('VAE decoding of palettized and dithered 8-bit latents')
    # print_metrics(gt_img, palettized_img)

    # # Use Stable Diffusion U-Net to de-noise the dithered latents
    # latents = denoise(latents)
    # denoised_img = to_img(latents)
    # denoised_img.show()
    # del latents
    # print('VAE decoding of de-noised dithered 8-bit latents')
    # print('size: {}b = {}kB'.format(sd_bytes, sd_bytes/1024.0))
    # print_metrics(gt_img, denoised_img)

    # Find JPG compression settings that result in closest data size that is larger than SD compressed data
    # jpg_bytes = io.BytesIO()
    # q = 0
    # while jpg_bytes.getbuffer().nbytes < sd_bytes:
    #     jpg_bytes = io.BytesIO()
    #     gt_img.save(jpg_bytes, format="JPEG", quality=q, optimize=True, subsampling=1)
    #     jpg_bytes.flush()
    #     jpg_bytes.seek(0)
    #     jpg_bytes = io.BytesIO(mozjpeg_lossless_optimization.optimize(jpg_bytes.read()))
    #     jpg_bytes.flush()
    #     q += 1

    # with open(output_path + ".jpg", "wb") as f:
    #     f.write(jpg_bytes.getbuffer())
    # jpg = Image.open(jpg_bytes)
    # try:
    #     jpg.show()
    #     print('JPG compressed with quality setting: {}'.format(q))
    #     print('size: {}b = {}kB'.format(jpg_bytes.getbuffer().nbytes, jpg_bytes.getbuffer().nbytes / 1024.0))
    #     print_metrics(gt_img, jpg)
    # except:
    #     print('something went wrong compressing {}.jpg'.format(output_path))

    # webp_bytes = io.BytesIO()
    # q = 0
    # while webp_bytes.getbuffer().nbytes < sd_bytes:
    #     webp_bytes = io.BytesIO()
    #     gt_img.save(webp_bytes, format="WEBP", quality=q, method=6)
    #     webp_bytes.flush()
    #     q += 1

    # with open(output_path + ".webp", "wb") as f:
    #     f.write(webp_bytes.getbuffer())
    # try:
    #     webp = Image.open(webp_bytes)
    #     webp.show()
    #     print('WebP compressed with quality setting: {}'.format(q))
    #     print('size: {}b = {}kB'.format(webp_bytes.getbuffer().nbytes, webp_bytes.getbuffer().nbytes / 1024.0))
    #     print_metrics(gt_img, webp)
    # except:
    #     print('something went wrong compressing {}.webp'.format(output_path))