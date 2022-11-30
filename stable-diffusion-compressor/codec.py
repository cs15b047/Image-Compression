huggingface_token = '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'
pretrained_model_name_or_path = "CompVis/stable-diffusion-v1-4"

from huggingface_hub import notebook_login

from diffusers import AutoencoderKL, UNet2DConditionModel, UNet2DModel, StableDiffusionImg2ImgPipeline
from transformers import CLIPFeatureExtractor, CLIPTextModel, CLIPTokenizer
from diffusers.schedulers import DDIMScheduler, LMSDiscreteScheduler, PNDMScheduler
import torch
from torch.cuda.amp import autocast

torch_device = "cpu"
# torch_device = "cuda"

vae = AutoencoderKL.from_pretrained(
    pretrained_model_name_or_path, subfolder="vae", use_auth_token=True
).to(torch_device)

unet = UNet2DConditionModel.from_pretrained(
    pretrained_model_name_or_path, subfolder="unet", use_auth_token=True
).to(torch_device)

scheduler = PNDMScheduler(
    beta_start=0.00085, beta_end=0.012, beta_schedule="scaled_linear",
    num_train_timesteps=1000, skip_prk_steps=True
)

text_encoder = CLIPTextModel.from_pretrained(
    pretrained_model_name_or_path, subfolder="text_encoder", use_auth_token=True,
)

tokenizer = CLIPTokenizer.from_pretrained(
    pretrained_model_name_or_path,
    subfolder="tokenizer",
    use_auth_token=True,
    torch_dtype=torch.float16
)

uncond_input = tokenizer([""], padding="max_length", max_length=tokenizer.model_max_length, return_tensors="pt")
with torch.no_grad():
  uncond_embeddings = text_encoder(uncond_input.input_ids)[0].to(torch_device)