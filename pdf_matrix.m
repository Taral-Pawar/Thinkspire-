% Step 1: Read and preprocess the image
img = imread('12th marksheet.jpg');
grayImg = rgb2gray(img);
bw = imbinarize(grayImg);      % Binary image (logical)

% Step 2: Resize for realistic printer resolution (optional)
bw = imresize(bw, [24 NaN]);   % Resize to 24 rows (pin height)

% Step 3: Convert to double (required by Simulink)
dotMatrix = double(bw);        % Still [24 x N] matrix

% Step 4: Convert to time series (each column is a sample in time)
ts = timeseries(dotMatrix', 1:size(dotMatrix,2));  % Transpose for Simulink!

% Step 5: Save as -v7.3
save('dotMatrixData.mat', 'ts', '-v7.3');
