import pandas as pd


def get_dataset_shape(df: pd.DataFrame) -> dict:
    return {
        "rows": df.shape[0],
        "columns": df.shape[1]
    }


def get_column_info(df: pd.DataFrame) -> pd.DataFrame:
    return pd.DataFrame({
        "Column": df.columns,
        "Data Type": df.dtypes.astype(str),
        "Non-Null Count": df.count().values,
        "Null Count": df.isnull().sum().values
    })


def get_descriptive_statistics(df: pd.DataFrame) -> pd.DataFrame:
    return df.describe()


def get_memory_usage(df: pd.DataFrame) -> float:
    return round(df.memory_usage(deep=True).sum() / 1024**2, 2)


def generate_summary(df: pd.DataFrame) -> dict:
    return {
        "shape": get_dataset_shape(df),
        "columns": get_column_info(df),
        "statistics": get_descriptive_statistics(df),
        "memory_usage_mb": get_memory_usage(df)
    }