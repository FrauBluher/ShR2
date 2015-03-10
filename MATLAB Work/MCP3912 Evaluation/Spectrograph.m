close all;
%[b a] = butter(3, 500/31250);
%y = filter(b, a, VarName2);
y = VarName2/max(abs(VarName2));

sig = y;

%sampling frequency
fs = 32000;

%signal duration
dur = length(y)/fs;

%time axis vector
t = linspace(0,dur,fs);

%fft length
N = 4096;

%hop size
hop = N/4;

%overlap
overlap = N - hop;

%take the STFT of the signal
S = spectrogram(sig,overlap);

%Max frequency to visualize
maxFreq = N/2;

%time vector
time = linspace(0,dur,size(S,2));

%frequency vector
freq= linspace(0,fs*maxFreq/N,size(S(1:maxFreq,:),1));

%set colour scale range (dB)
clims = [0 50];

%plot the STFT heatmap
fig = figure;
imagesc(time,freq,20*log10(abs(S(1:maxFreq,:))),clims)
colorbar
axis xy
xlabel('TIME (s.)')
ylabel('FREQUENCY (Hz.)')
title(['sead Short Time Fourier Analysis']);