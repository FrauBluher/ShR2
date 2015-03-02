close all;
%% MCP3912 Initial Testing
% Linear Sweep 0-8kHz
Y = fft(VarName5, 2^12);
Pyy = Y.*conj(Y) / 2^12;
f = 35200/4096*(0:((2^12)/2)-1);

loglog(f, mag2db(Pyy(1:(2^12)/2)))

title('PSD: 0-20kHz Square Wave Sweep Excitation of CT');
xlabel('Frequency (Hz)');
ylabel('dB');