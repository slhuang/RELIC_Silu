#!/bin/python

import pandas as pd
import numpy as np

import traceback

from lineage import LineageTracker


class PandasVersionGenerator:

    def __init__(self, shape, minVal=0,
                 maxVal=100, directory='dataset/', scale=10.):
        self.minVal = minVal
        self.maxVal = maxVal

        rowsize, colsize = shape
        self.dataset = []
        base_array = np.random.randint(minVal, high=maxVal,
                                       size=(rowsize, colsize))
        base_df = pd.DataFrame(base_array,
                               columns=[str(x) for x in range(colsize)],
                               index=np.arange(rowsize))

        self.lineage = LineageTracker(directory)
        self.dataset.append(base_df)
        self.lineage.new_item(self.get_last_label(), base_df)
        self.scale = scale

    def __getitem__(self, item):
        return self.dataset[item]

    def __len__(self):
        return len(self.dataset)

    def select_rand_dataset(self, for_merge=False):
        size = len(self.dataset)
        i = np.arange(size)  # an array of the index value for weighting
        prob = np.exp(i/self.scale)  # higher weights for larger index values
        prob /= prob.sum()

        if(for_merge):
            if(size < 2):
                return None
            elif(size == 2):
                return [0, 1]
            else:
                choice = np.random.choice(i, 2, p=prob,
                                          replace=False)
        else:
            if(size < 2):
                return 0
            choice = np.random.choice(i, 1, p=prob)[0]

        return choice

    def apply_op(self, op_function, **kwargs):
        if op_function == merge:  # Merge is special case
            choice = self.select_rand_dataset(for_merge=True)
            if choice is None:  # Not enough generated datasets for merge
                pass

            df1 = self.dataset[choice[0]]
            df2 = self.dataset[choice[1]]
            new_df = merge(df1, df2).dropna()
            self.lineage.new_item(str(len(self.dataset)), new_df)
            self.dataset.append(new_df)
            self.lineage.link(str(choice[0]), self.get_last_label(),
                              str(op_function).split()[1])
            self.lineage.link(str(choice[1]), self.get_last_label(),
                              str(op_function).split()[1])

        else:
            choice = self.select_rand_dataset()
            base_df = self.dataset[choice]
            new_df = op_function(base_df, **kwargs).dropna()
            if new_df.empty:
                raise pd.errors.EmptyDataError
            self.lineage.new_item(str(len(self.dataset)), new_df)
            self.dataset.append(new_df)
            self.lineage.link(str(choice), self.get_last_label(),
                              str(op_function).split()[1])

    def get_last_label(self):
        return str(len(self.dataset)-1)


def select_rand_aggregate():
    return np.random.choice(['min', 'max', 'sum', 'mean'], 1)[0]


def agg(df):
    new_df = df.agg(select_rand_aggregate()).to_frame(name="_agg")
    return new_df


def assign(df):
    col = select_rand_cols(df)[0]
    # TODO: Randomize assignment function
    new_col_name = str(col)+"_assign"
    # Dynamic keyword expansion in python below:
    return df.assign(**{new_col_name: lambda x: np.power(x[col], 3)}) #np.exp(x[col])


def expanding(df):
    window = get_rand_window()
    func = select_rand_aggregate()
    method = getattr(df.expanding(window), func)
    return method()


def groupby(df):
    col = select_rand_cols(df)[0]
    func = select_rand_aggregate()
    method = getattr(df.groupby(col), func)
    return method()


def iloc(df):
    # Select random row slice
    num1 = np.random.randint(0, len(df.index))
    num2 = np.random.randint(num1, len(df.index))
    return df.iloc[num1:num2]


def melt(df):
    id_col, val_col = select_rand_cols(df, 2)
    return pd.melt(df, id_vars=[id_col],
                   value_vars=[val_col],
                   var_name='_variable',
                   value_name='_value')


def merge(df1, df2):
    return df1.merge(df2)


def nlargest(df):
    n = np.random.randint(1, len(df.index))
    col = select_rand_cols(df)[0]
    return df.nlargest(n, col)


def nsmallest(df):
    n = np.random.randint(1, len(df.index))
    col = select_rand_cols(df)[0]
    return df.nsmallest(n, col)


def reindex(df):
    return df.reindex(get_row_permutation())


def rolling(df):
    window = get_rand_window()
    func = select_rand_aggregate()
    method = getattr(df.rolling(window), func)
    return method()


def sample(df):
    return df.sample(frac=get_rand_percentage())


def sort_index(df):
    return df.sort_index()


def sort_values(df):
    col = select_rand_cols(df)[0]
    choice = np.random.choice([True, False], 1)[0]
    return df.sort_values(by=col, ascending=choice)


def drop_cols(df):
    cols = select_rand_cols(df)
    return df.drop(columns=cols)


def drop_rows(df):
    # Atmost 1/2 rows are dropped
    if df.index < 3:
        to_drop = 1
    else:
        to_drop = np.random.randint(1, len(df.index)/2)
    rows = select_rand_rows(df, num=to_drop)
    return df.drop(rows)


def select_rand_rows(df, num=1):
    return np.random.choice(df.index.values, num)


def select_rand_cols(df, num=1):
    return np.random.choice(df.columns.values, num)


def get_rand_window(minimum=2, maximum=10):
    return np.random.randint(minimum, maximum)


def get_rand_percentage(minimum=0.01, maximum=0.99):
    return round(np.random.random_sample(), 2)


def get_row_permutation(df):
    return np.random.permutation(df.index.values)


def get_rand_aggregate():
    return np.random.choice(['min', 'max', 'sum', 'avg'], 1)[0]


def generate__dataset(shape, n, scale=10):
    operations = [
        #agg,       ### non-preserving
        assign,
        expanding,
        #groupby,   ### non-preserving
        iloc,
        # melt,
        #merge,     ### non-preserving
        nlargest,
        nsmallest,
        reindex,   ### this may cause a problem??
        rolling,
        sample,
        sort_index,
        sort_values,
        drop_cols,
        drop_rows,
    ]

    dataset = PandasVersionGenerator(shape, scale=scale)

    errors = []

    i = 0

    while i != n-1:
        choice = np.random.choice(operations, 1)[0]
        try:
            dataset.apply_op(choice)
            i += 1
        except Exception as e:
            tb = traceback.format_exc()
            errors.append({str(choice).split()[1]: tb})

    return dataset, errors
