import numpy as np
import pandas as pd


# ==========================================================
# MISSING VALUE HANDLING
# ==========================================================

def remove_missing_rows(
    df: pd.DataFrame,
    row_threshold: float = 0.5
) -> pd.DataFrame:
    """
    Handle missing values while minimizing data loss.

    Completely empty columns are removed first.

    Rows are removed only when their missing-value
    ratio is greater than the given threshold.

    Remaining numeric missing values are filled
    with the column median.

    Remaining categorical missing values are filled
    with the column mode.

    Parameters
    ----------
    df : pandas.DataFrame
        Input dataset.

    row_threshold : float
        Maximum allowed missing ratio per row.
        Default is 0.5 (50%).

    Returns
    -------
    pandas.DataFrame
        Cleaned dataset.
    """

    cleaned_df = df.copy()

    # ------------------------------------------------------
    # 1. Validate threshold
    # ------------------------------------------------------

    if not 0 <= row_threshold <= 1:
        raise ValueError(
            "row_threshold must be between 0 and 1."
        )

    # ------------------------------------------------------
    # 2. Remove completely empty columns
    # ------------------------------------------------------

    empty_columns = [
        column
        for column in cleaned_df.columns
        if cleaned_df[column].isna().all()
    ]

    if empty_columns:
        cleaned_df = cleaned_df.drop(
            columns=empty_columns
        )

    # ------------------------------------------------------
    # 3. Calculate missing ratio for each row
    # ------------------------------------------------------

    if cleaned_df.empty:
        return cleaned_df

    missing_ratio = cleaned_df.isna().mean(axis=1)

    # ------------------------------------------------------
    # 4. Remove only highly incomplete rows
    # ------------------------------------------------------

    cleaned_df = cleaned_df.loc[
        missing_ratio <= row_threshold
    ].copy()

    # ------------------------------------------------------
    # 5. Remove columns that became completely empty
    #    after row filtering
    # ------------------------------------------------------

    empty_columns_after_filter = [
        column
        for column in cleaned_df.columns
        if cleaned_df[column].isna().all()
    ]

    if empty_columns_after_filter:
        cleaned_df = cleaned_df.drop(
            columns=empty_columns_after_filter
        )

    # ------------------------------------------------------
    # 6. Fill numeric missing values with median
    # ------------------------------------------------------

    numeric_columns = cleaned_df.select_dtypes(
        include=np.number
    ).columns

    for column in numeric_columns:

        if cleaned_df[column].isna().any():

            median_value = cleaned_df[column].median()

            if not pd.isna(median_value):

                cleaned_df[column] = (
                    cleaned_df[column]
                    .fillna(median_value)
                )

    # ------------------------------------------------------
    # 7. Fill categorical missing values with mode
    # ------------------------------------------------------

    categorical_columns = cleaned_df.select_dtypes(
        exclude=np.number
    ).columns

    for column in categorical_columns:

        if cleaned_df[column].isna().any():

            mode = cleaned_df[column].mode()

            if not mode.empty:

                cleaned_df[column] = (
                    cleaned_df[column]
                    .fillna(mode.iloc[0])
                )

    # ------------------------------------------------------
    # 8. Final safety check
    # ------------------------------------------------------

    remaining_empty_columns = [
        column
        for column in cleaned_df.columns
        if cleaned_df[column].isna().all()
    ]

    if remaining_empty_columns:
        cleaned_df = cleaned_df.drop(
            columns=remaining_empty_columns
        )

    return cleaned_df


# ==========================================================
# MISSING VALUE - MEAN
# ==========================================================

def fill_missing_mean(
    df: pd.DataFrame
) -> pd.DataFrame:
    """
    Fill missing numeric values with column mean.
    """

    cleaned_df = df.copy()

    numeric_columns = cleaned_df.select_dtypes(
        include=np.number
    ).columns

    for column in numeric_columns:

        if cleaned_df[column].isna().any():

            mean_value = cleaned_df[column].mean()

            if not pd.isna(mean_value):

                cleaned_df[column] = (
                    cleaned_df[column]
                    .fillna(mean_value)
                )

    return cleaned_df


# ==========================================================
# MISSING VALUE - MEDIAN
# ==========================================================

def fill_missing_median(
    df: pd.DataFrame
) -> pd.DataFrame:
    """
    Fill missing numeric values with column median.
    """

    cleaned_df = df.copy()

    numeric_columns = cleaned_df.select_dtypes(
        include=np.number
    ).columns

    for column in numeric_columns:

        if cleaned_df[column].isna().any():

            median_value = cleaned_df[column].median()

            if not pd.isna(median_value):

                cleaned_df[column] = (
                    cleaned_df[column]
                    .fillna(median_value)
                )

    return cleaned_df


# ==========================================================
# MISSING VALUE - MODE
# ==========================================================

def fill_missing_mode(
    df: pd.DataFrame
) -> pd.DataFrame:
    """
    Fill missing values with column mode.
    """

    cleaned_df = df.copy()

    for column in cleaned_df.columns:

        if cleaned_df[column].isna().any():

            mode = cleaned_df[column].mode()

            if not mode.empty:

                cleaned_df[column] = (
                    cleaned_df[column]
                    .fillna(mode.iloc[0])
                )

    return cleaned_df


# ==========================================================
# DUPLICATE HANDLING
# ==========================================================

def remove_duplicates(
    df: pd.DataFrame
) -> pd.DataFrame:
    """
    Remove duplicate rows.
    """

    return df.drop_duplicates()


# ==========================================================
# IQR HELPER
# ==========================================================

def _get_iqr_bounds(
    series: pd.Series,
    multiplier: float = 1.5
):
    """
    Calculate IQR lower and upper bounds.

    Non-finite values are ignored while calculating
    the bounds.
    """

    clean_series = series[
        np.isfinite(series)
    ].dropna()

    if clean_series.empty:
        return None, None

    if clean_series.nunique() <= 1:
        return None, None

    q1 = clean_series.quantile(0.25)
    q3 = clean_series.quantile(0.75)

    iqr = q3 - q1

    if pd.isna(iqr) or iqr == 0:
        return None, None

    lower = q1 - multiplier * iqr
    upper = q3 + multiplier * iqr

    return lower, upper


# ==========================================================
# OUTLIER HANDLING - REMOVE
# ==========================================================

def remove_outliers_iqr(
    df: pd.DataFrame,
    columns: list,
    multiplier: float = 1.5
) -> pd.DataFrame:
    """
    Remove rows containing at least one IQR outlier.

    Outlier boundaries are calculated for all columns
    before rows are removed.

    This prevents excessive data loss caused by
    sequential row deletion.
    """

    cleaned_df = df.copy()

    outlier_mask = pd.Series(
        False,
        index=cleaned_df.index
    )

    for column in columns:

        if column not in cleaned_df.columns:
            continue

        series = cleaned_df[column]

        if not pd.api.types.is_numeric_dtype(
            series
        ):
            continue

        lower, upper = _get_iqr_bounds(
            series,
            multiplier
        )

        if lower is None or upper is None:
            continue

        column_mask = (
            (series < lower) |
            (series > upper)
        )

        outlier_mask = (
            outlier_mask |
            column_mask.fillna(False)
        )

    cleaned_df = cleaned_df.loc[
        ~outlier_mask
    ].copy()

    return cleaned_df


# ==========================================================
# OUTLIER HANDLING - MEAN
# ==========================================================

def replace_outliers_mean(
    df: pd.DataFrame,
    columns: list,
    multiplier: float = 1.5
) -> pd.DataFrame:
    """
    Replace IQR outliers with the column mean.
    """

    cleaned_df = df.copy()

    for column in columns:

        if column not in cleaned_df.columns:
            continue

        series = cleaned_df[column]

        if not pd.api.types.is_numeric_dtype(
            series
        ):
            continue

        lower, upper = _get_iqr_bounds(
            series,
            multiplier
        )

        if lower is None or upper is None:
            continue

        finite_values = series[
            np.isfinite(series)
        ]

        if finite_values.empty:
            continue

        mean_value = finite_values.mean()

        mask = (
            (series < lower) |
            (series > upper)
        )

        cleaned_df.loc[
            mask.fillna(False),
            column
        ] = mean_value

    return cleaned_df


# ==========================================================
# OUTLIER HANDLING - MEDIAN
# ==========================================================

def replace_outliers_median(
    df: pd.DataFrame,
    columns: list,
    multiplier: float = 1.5
) -> pd.DataFrame:
    """
    Replace IQR outliers with the column median.
    """

    cleaned_df = df.copy()

    for column in columns:

        if column not in cleaned_df.columns:
            continue

        series = cleaned_df[column]

        if not pd.api.types.is_numeric_dtype(
            series
        ):
            continue

        lower, upper = _get_iqr_bounds(
            series,
            multiplier
        )

        if lower is None or upper is None:
            continue

        finite_values = series[
            np.isfinite(series)
        ]

        if finite_values.empty:
            continue

        median_value = finite_values.median()

        mask = (
            (series < lower) |
            (series > upper)
        )

        cleaned_df.loc[
            mask.fillna(False),
            column
        ] = median_value

    return cleaned_df


# ==========================================================
# OUTLIER HANDLING - WINSORIZE
# ==========================================================

def winsorize_outliers(
    df: pd.DataFrame,
    columns: list,
    multiplier: float = 1.5
) -> pd.DataFrame:
    """
    Winsorize IQR outliers by clipping values
    to the calculated lower and upper bounds.
    """

    cleaned_df = df.copy()

    for column in columns:

        if column not in cleaned_df.columns:
            continue

        series = cleaned_df[column]

        if not pd.api.types.is_numeric_dtype(
            series
        ):
            continue

        lower, upper = _get_iqr_bounds(
            series,
            multiplier
        )

        if lower is None or upper is None:
            continue

        cleaned_df[column] = series.clip(
            lower=lower,
            upper=upper
        )

    return cleaned_df