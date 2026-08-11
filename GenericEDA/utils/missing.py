import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns


def _missing_summary(df):
    """
    Creates a summary table of missing values.
    """

    missing_count = df.isnull().sum()
    missing_percent = (missing_count / len(df)) * 100

    summary = pd.DataFrame({
        "Column": missing_count.index,
        "Missing Count": missing_count.values,
        "Missing %": missing_percent.values.round(2)
    })

    summary = summary.sort_values(
        by="Missing %",
        ascending=False
    ).reset_index(drop=True)

    return summary



def plot_missing_bar(summary):
    """
    Draws bar chart for missing values.
    """

    summary = summary[summary["Missing Count"] > 0]

    if summary.empty:
        print("No missing values to visualize.")
        return

    plt.figure(figsize=(12,5))

    plt.bar(
        summary["Column"],
        summary["Missing Count"]
    )

    plt.xticks(rotation=90)

    plt.xlabel("Columns")
    plt.ylabel("Missing Count")
    plt.title("Missing Values by Column")

    plt.tight_layout()

    plt.show()


def plot_missing_heatmap(df):
    """
    Displays missing value heatmap.
    """

    plt.figure(figsize=(12,6))

    sns.heatmap(
        df.isnull(),
        cbar=False,
        yticklabels=False
    )

    plt.title("Missing Value Heatmap")

    plt.show()


def analyze_missing(df):
    """
    Main function for missing value analysis.
    """

    summary = _missing_summary(df)

    return summary