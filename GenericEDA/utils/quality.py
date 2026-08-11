import numpy as np
import pandas as pd


def check_missing_values(df: pd.DataFrame) -> pd.DataFrame:
    return pd.DataFrame({
        "Missing Count": df.isnull().sum(),
        "Missing Percentage": (df.isnull().sum() / len(df) * 100).round(2)
    })


def check_duplicate_rows(df: pd.DataFrame) -> dict:
    duplicate_count = int(df.duplicated().sum())

    return {
        "Duplicate Count": duplicate_count,
        "Duplicate Percentage": round((duplicate_count / len(df)) * 100, 2)
    }


def check_infinite_values(df: pd.DataFrame) -> pd.DataFrame:
    numeric_df = df.select_dtypes(include=[np.number])

    return pd.DataFrame({
        "Infinite Count": np.isinf(numeric_df).sum()
    })


def check_constant_columns(df: pd.DataFrame) -> list:
    return [
        column
        for column in df.columns
        if df[column].nunique(dropna=False) == 1
    ]


def check_empty_columns(df: pd.DataFrame) -> list:
    return [
        column
        for column in df.columns
        if df[column].isnull().all()
    ]


def check_data_types(df: pd.DataFrame) -> pd.DataFrame:
    return pd.DataFrame({
        "Column": df.columns,
        "Data Type": df.dtypes.astype(str)
    })


def count_column_types(df: pd.DataFrame) -> dict:
    numeric_columns = df.select_dtypes(include=[np.number]).shape[1]
    categorical_columns = df.select_dtypes(exclude=[np.number]).shape[1]

    return {
        "Numeric Columns": numeric_columns,
        "Categorical Columns": categorical_columns
    }


def generate_quality_report(df: pd.DataFrame) -> dict:
    return {
        "dataset_shape": {
            "Rows": df.shape[0],
            "Columns": df.shape[1]
        },
        "missing_values": check_missing_values(df),
        "duplicate_rows": check_duplicate_rows(df),
        "infinite_values": check_infinite_values(df),
        "constant_columns": check_constant_columns(df),
        "empty_columns": check_empty_columns(df),
        "data_types": check_data_types(df),
        "column_types": count_column_types(df)
    }