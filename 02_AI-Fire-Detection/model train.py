import torch
from ultralytics import YOLO


def train():
    print("CUDA available?", torch.cuda.is_available())
    if torch.cuda.is_available():
        print("Using GPU:", torch.cuda.get_device_name(torch.cuda.current_device()))

    model = YOLO("yolo1.pt")

    model.train(
        data="data.yaml",
        epochs=50,
        imgsz=640,
        batch=8,
        device='cuda:0',
        project="runs/fire_smoke",
        name='fire-and-smoke-finetune',
        workers=4
    )


if __name__ == "__main__":
    train()