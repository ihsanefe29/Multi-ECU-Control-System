from pathlib import Path

import pandas as pd


SUPPORTED_FORMATS = (".xlsx", ".xls", ".csv", ".txt")


def load_dataset(file_path: str) -> pd.DataFrame:
    path = Path(file_path)

    if not path.exists():
        raise FileNotFoundError(f"File not found: {file_path}")

    extension = path.suffix.lower()

    if extension not in SUPPORTED_FORMATS:
        raise ValueError(
            f"Unsupported file format: {extension}\n"
            f"Supported formats: {', '.join(SUPPORTED_FORMATS)}"
        )

    try:
        if extension in [".xlsx", ".xls"]:
            df = pd.read_excel(file_path)

        elif extension == ".csv":
            try:
                df = pd.read_csv(file_path, encoding="utf-8")
            except UnicodeDecodeError:
                df = pd.read_csv(file_path, encoding="latin1")

        elif extension == ".txt":
            df = pd.read_csv(file_path, sep=r"\s+", header=None)

        if df.empty:
            raise ValueError("The dataset is empty.")

        return df

    except Exception as error:
        raise Exception(f"Dataset could not be loaded.\n{error}")