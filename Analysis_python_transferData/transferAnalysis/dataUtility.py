

"""
this module is for preparing the data for analysis.
creates one consolidated image for the transfer analytics
transfering plots and pdf
"""


import os
import re
import argparse
import glob
from pathlib import Path
import shutil
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.ticker as ticker



def parse_args():
    parser = argparse.ArgumentParser(description='Data Utility for preprocessing and plotting analysis') 

    parser.add_argument('--source_path', type=str, default='data', help='Path to the source folder')
    parser.add_argument('--max_data', type=int, default = 4_00_00_00, help='Maximum data transfered')

    args = parser.parse_args()
    return args


printed = False
def transferPdfAnalysis(resultFolder, pdfSourceDataPath = None):
    """Folder name : resultFolder
    Subfolder 1: ajapath to the source folder
    """

    if pdfSourceDataPath == None:
        pdfSourceDataPath = os.getcwd()
        pdfs = Path(pdfSourceDataPath).glob("*.pdf")
    else:
        pdfs = Path(pdfSourceDataPath).glob("*.pdf")

    # resultPath = os.getcwd() + f'/{resultFolder}'
    resultPath = os.path.join(os.getcwd(), resultFolder)
    os.makedirs(resultPath, exist_ok=True) 
    
    global printed
    for pdf in pdfs:
        # print(pdf)
        pdfName = str(pdf).split("/")[-1:]
        pdfName = re.sub(r"\"|\[|\'|\]", "", str(pdfName))
        compiler = pdfName.split('.')[0].split('_')[-1]
        library = pdfName.split('.')[0].split('_')[-2]
        # print(f'Compiler: {compiler} Library: {library}')

        validSubFolders = ['openMPIProcess', 'intelMPIProcess', 
                           'gnuCompiler', 'intelCompiler']
        if library not in validSubFolders or compiler not in validSubFolders:
            print(f'Invalid sub folder library->{library} and compiler->{compiler} used in {pdf}')
            continue
        
        if library in ['openMPIProcess', 'intelMPIProcess'] and compiler in ['gnuCompiler', 'intelCompiler']:
            subFolderPath = os.path.join(resultPath, library, compiler)
            # print(subFolderPath)
            os.makedirs(subFolderPath, exist_ok=True)
        else:
            print(f'UnIdentified {compiler}/{library} name used in {pdf}')
            # pass
        # break
        
        shutil.move(pdf, subFolderPath)



def prepCsvData(path, minData = 0):
    
    """Reads all csv files in a folder and returns a dataframe with the data
    from all the files. The dataframe is filtered to only include data between
    minData and maxData.
    
    Args:
        path (str) : Path to the folder containing the csv files.
        maxData (int): Maximum data transfered
        minData (int): Minimum data transfered
    Returns:
        df (pandas.DataFrame): Dataframe with the data from all the csv files
        """

    csv_files = glob.glob(os.path.join(path, "*.csv"))

    df = pd.concat([pd.read_csv(file).iloc[10:] for file in csv_files ], 
                   axis = 0, ignore_index=True)
    df.dropna(axis=0, inplace = True)

    df['BytesData(B)'] = df['BytesData(B)']#.astype(int)
    df = df[(df['BytesData(B)'] > minData)] #&(df['BytesData'] <= maxData)] 
    df['Bits'] = df['BytesData(B)']*8
    return df

def prepDicDataFrame(fileName, df):
    
    """
    _summary_: Prepares a dictionary with one dataframe

    Args:
        fileName (string): Name of the file
        df (dataframe): Dataframe to be saved

    Returns:
        dictionary: Dictionary with one dataframe
    """
    fileName = re.sub(r"\"|\[|\'|\]", "", str(fileName))
    data_ = {f'{fileName}':df}
    return data_


def extractFiles(source_path):

    """ 
        Extracts the CSV files from the source path

        Args:
            source_path (string): Path to the source folder

        Returns:
            dictionary: Dictionary with all the dataframes
        
    """
    # Check if the source path exists
    if not os.path.exists(source_path):
        print(f"Source path {source_path} does not exist.")
        return
    
    # Extract folders containing CSV files
    data_ = {}
    csvFilePath = []
    for root, directories, _ in os.walk(source_path):
        for directory in directories:
            directory_path = os.path.join(root, directory)
            # Check if the directory contains CSV files
            has_csv_files = False
            for filename in os.listdir(directory_path):
                if filename.endswith('.csv'):
                    has_csv_files = True
                    break

            if has_csv_files:
                csvFilePath.append(directory_path)

    
    for csvFile in csvFilePath:
        print(f"csv data for {csvFile.split('/')[-1:]} in {csvFile}")
        data_.update(prepDicDataFrame(fileName = csvFile.split('/')[-1:],
                                      df = prepCsvData(csvFile, minData = 0)))

    return data_, csvFilePath

def main():
    print('hello world')
    args = parse_args()
    print(args)
    extractFiles(args.source_path)



if __name__ == '__main__':
    main()