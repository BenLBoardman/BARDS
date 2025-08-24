import pandas as pd

class Precinct:
    def __init__(self, precinct: pd.Series):
        self.pop = precinct['TOTPOP']
        self.neighbors = precinct['neighbors']
        self.index = precinct['index']