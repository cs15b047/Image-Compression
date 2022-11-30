import PIL
from PIL import Image
import numpy as np
import inspect
import io
import libimagequant as liq
import zlib
import gc
import time
import mozjpeg_lossless_optimization
from skimage.metrics import structural_similarity as get_ssim
from skimage.metrics import peak_signal_noise_ratio as get_psnr

from codec import *

@torch.no_grad()
def to_latents(img:Image):
  np_img = (np.array(img).astype(np.float32) / 255.0) * 2.0 - 1.0
  np_img = np_img[None].transpose(0, 3, 1, 2)
  torch_img = torch.from_numpy(np_img)
  with autocast():
    generator = torch.Generator("cpu").manual_seed(0)
    latents = vae.encode(torch_img.to(vae.dtype).to(torch_device)).latent_dist.sample(generator=generator)
  return latents

@torch.no_grad()
def to_img(latents):
  with autocast():
    torch_img = vae.decode(latents.to(vae.dtype).to(torch_device)).sample
  torch_img = (torch_img / 2 + 0.5).clamp(0, 1)
  np_img = torch_img.cpu().permute(0, 2, 3, 1).detach().numpy()[0]
  np_img = (np_img * 255.0).astype(np.uint8)
  img = Image.fromarray(np_img)
  return img

def resize_to_512(input_file):
  img = Image.open(input_file).convert('RGB')
  #center crop image
  maxdim = max(img.width, img.height)
  mindim = min(img.width, img.height)
  left = max(0, (img.width - img.height) // 2 - 1)
  top = max(0, (img.height - img.width) // 2 - 1)
  # img = img.crop((left, top, left + mindim - 1, top + mindim - 1))
  #resize
  img = img.resize((512,512), Image.LANCZOS)
  # img.save(output_file, lossless = True, quality = 100)
  return img

def print_metrics(gt, img):
  gt = np.array(gt)
  img = np.array(img)
  print('PSNR: ' + str(get_psnr(gt, img)))
  print('SSIM: ' + str(get_ssim(gt, img, multichannel=True, data_range=img.max() - img.min())))