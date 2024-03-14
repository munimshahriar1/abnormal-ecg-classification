Context
A beginner-friendly version of the MIT-BIH Arrhythmia Database, which contains 48 electrocardiograms (EKGs) from 47 (208.csv is data of Patient 208) patients that were at Beth Israel Deaconess Medical Center in Boston, MA in 1975-1979.

There are 48 CSVs, each of which is a 30-minute echocardiogram (EKG) from a single patient (record 201 and 202 are from the same patient). Data was collected at 360 Hz, meaning that 360 rows is equal to 1 second of time.

Content
Raw signals in .csv files and original annotations in .txt.
Structure .csv files
number_of_sample, raw_value_signal_1, raw_value_signal_2

Note
Lead MLII is the lead most often associated with the classic QRS Complex (the medical name for a single heartbeat).


Acknowledgements
The original data can be found in https://www.physionet.org/physiobank/database/mitdb/