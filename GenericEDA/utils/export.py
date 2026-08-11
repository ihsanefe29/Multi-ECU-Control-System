from datetime import datetime
from pathlib import Path

import pandas as pd


def export_dataset(
    df: pd.DataFrame,
    extension: str
):
    extension = extension.lower()

    if extension not in [".csv", ".xlsx"]:
        raise ValueError(
            "Only .xlsx and .csv formats are supported."
        )

    output_dir = Path("outputs")
    output_dir.mkdir(exist_ok=True)

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")

    output_path = output_dir / f"cleaned_dataset_{timestamp}{extension}"

    if extension == ".csv":
        df.to_csv(output_path, index=False)

    else:
        df.to_excel(output_path, index=False)

    print("✅ Dataset exported successfully.")
    print(f"📁 Saved to: {output_path}")