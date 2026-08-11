"""
session.py

Session management module for GenericEDA.

Features
--------
- Cleaning history / log
- Undo last operation
- Before / After comparison
"""

from datetime import datetime

import pandas as pd


class EDASession:
    """
    Keeps track of dataset changes during an EDA session.
    """

    def __init__(self, df: pd.DataFrame):

        self.original_df = df.copy(deep=True)

        self.current_df = df.copy(deep=True)

        self.history = []

        self.undo_stack = []


    # ======================================================
    # SYNC
    # ======================================================

    def sync(self, df):
        """
        Synchronizes session with an external DataFrame.
        """

        self.current_df = df.copy(
            deep=True
        )

        print(
            f"Session synchronized. "
            f"Current shape: {self.current_df.shape}"
        )


    # ======================================================
    # INTERNAL SNAPSHOT
    # ======================================================

    def _create_snapshot(self):
        """
        Stores current DataFrame before a cleaning operation.
        """

        self.undo_stack.append(
            self.current_df.copy(deep=True)
        )


    # ======================================================
    # APPLY OPERATION
    # ======================================================

    def apply(
        self,
        new_df: pd.DataFrame,
        operation: str,
        details: str = ""
    ):
        """
        Applies a new DataFrame state and records the operation.
        """

        self._create_snapshot()

        before_rows = len(self.current_df)
        before_columns = len(self.current_df.columns)

        after_rows = len(new_df)
        after_columns = len(new_df.columns)

        removed_rows = (
            before_rows - after_rows
        )

        removed_columns = (
            before_columns - after_columns
        )

        self.current_df = new_df.copy(
            deep=True
        )

        log_entry = {
            "Time": datetime.now().strftime(
                "%H:%M:%S"
            ),
            "Operation": operation,
            "Details": details,
            "Rows Before": before_rows,
            "Rows After": after_rows,
            "Rows Removed": removed_rows,
            "Columns Before": before_columns,
            "Columns After": after_columns,
            "Columns Removed": removed_columns
        }

        self.history.append(
            log_entry
        )

        print(
            f"\n✅ Operation recorded: "
            f"{operation}"
        )


    # ======================================================
    # UNDO
    # ======================================================

    def undo(self):
        """
        Restores the dataset to its previous state.
        """

        if not self.undo_stack:

            print(
                "\n⚠️ Nothing to undo."
            )

            return self.current_df

        previous_df = (
            self.undo_stack.pop()
        )

        current_rows = len(
            self.current_df
        )

        previous_rows = len(
            previous_df
        )

        self.current_df = (
            previous_df.copy(
                deep=True
            )
        )

        self.history.append({
            "Time": datetime.now().strftime(
                "%H:%M:%S"
            ),
            "Operation": "UNDO",
            "Details": (
                "Previous dataset state restored."
            ),
            "Rows Before": current_rows,
            "Rows After": previous_rows,
            "Rows Removed": (
                current_rows - previous_rows
            ),
            "Columns Before": "-",
            "Columns After": "-",
            "Columns Removed": "-"
        })

        print(
            "\n↩️ Last operation undone."
        )

        print(
            f"Current shape: "
            f"{self.current_df.shape}"
        )

        return self.current_df


    # ======================================================
    # HISTORY
    # ======================================================

    def show_history(self):
        """
        Displays cleaning operation history.
        """

        if not self.history:

            print(
                "\nNo operations recorded yet."
            )

            return pd.DataFrame()

        history_df = pd.DataFrame(
            self.history
        )

        print(
            "\n" + "=" * 80
        )

        print(
            "CLEANING HISTORY"
        )

        print(
            "=" * 80
        )

        print(
            history_df.to_string(
                index=False
            )
        )

        return history_df


    # ======================================================
    # BEFORE / AFTER COMPARISON
    # ======================================================

    def comparison(self, df=None):
        """
        Compares original dataset with the latest dataset.

        If df is provided, comparison is made against that
        DataFrame. Otherwise current_df is used.
        """

        original = self.original_df

        if df is not None:
            current = df.copy(deep=True)
        else:
            current = self.current_df

        original_missing = int(
            original.isnull().sum().sum()
        )

        current_missing = int(
            current.isnull().sum().sum()
        )

        original_duplicates = int(
            original.duplicated().sum()
        )

        current_duplicates = int(
            current.duplicated().sum()
        )

        removed_rows = (
            len(original) - len(current)
        )

        removed_columns = (
            len(original.columns)
            - len(current.columns)
        )

        data_loss = (
            removed_rows / len(original) * 100
            if len(original)
            else 0
        )

        comparison_df = pd.DataFrame({
            "Metric": [
                "Rows",
                "Columns",
                "Missing Values",
                "Duplicate Rows"
            ],

            "Before": [
                len(original),
                len(original.columns),
                original_missing,
                original_duplicates
            ],

            "After": [
                len(current),
                len(current.columns),
                current_missing,
                current_duplicates
            ]
        })

        print("\n" + "=" * 60)
        print("BEFORE / AFTER COMPARISON")
        print("=" * 60)

        print(
            comparison_df.to_string(
                index=False
            )
        )

        print(
            f"\nRows Removed    : {removed_rows}"
        )

        print(
            f"Columns Removed : {removed_columns}"
        )

        print(
            f"Data Loss       : {data_loss:.2f}%"
        )

        return comparison_df