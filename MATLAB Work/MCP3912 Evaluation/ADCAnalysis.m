close all;
%% MCP3912 Initial Testing
% Linear Sweep 0-8kHz
Y = fft(LinearSweep, 4096);
Pyy = Y.*conj(Y) / 4096;
f = 16000/4096*(0:2047);

plot(f, Pyy(1:2048))

title('Power spectral density: Sweep 0 - 8 kHz');
xlabel('Frequency (Hz)');
figure;

%%
%Agilent "White-ish Noise"
Y = fft(Noise, 4096);
Pyy = Y.*conj(Y) / 4096;

plot(f, Pyy(1:2048))

title('Power spectral density: Agilent Noise');
xlabel('Frequency (Hz)');
figure

%%
%Spectral noise on shorted inputs.
Y = fft(Shorted, 4096);
Pyy = Y.*conj(Y) / 4096;

plot(f, Pyy(1:2048))

title('Power spectral density: Shorted Input');
xlabel('Frequency (Hz)');
figure;

%%
% Spectral density for sine.
Y = fft(Sine100HzCh2, 4096);
Pyy = Y.*conj(Y) / 4096;

plot(f, Pyy(1:2048))

title('Power spectral density: Sine');
xlabel('Frequency (Hz)');

