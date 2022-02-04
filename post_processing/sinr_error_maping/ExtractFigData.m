clf
clear
clc

% Figure to extract data
open('LTE-V2X_LOS-fading.fig');


%% Data extraction from figure
D=get(gca,'Children'); %get the handle of the line object
XData=get(D,'XData'); %get the x data
YData=get(D,'YData'); %get the y data
h1 = findobj(gcf, 'tag', 'legend');
ltext = get(h1,'string');

%% Covert SNR to corresponding distance

for i=1:length(D)
 EsN0(i,:) = XData{length(D)-i+1}; % Es/N0 [dB]
 per(i,:) = YData{length(D)-i+1};  % Packet Error rate
 
 idx = EsN0==0;
 EsN0(idx) = NaN;
 per(idx) = NaN;
end

%% Figure

figure
for i=1:length(D)
    plot(EsN0(i,:),per(i,:),'-o')
    hold on
end
hold off
xlabel('Es/N0 [dB]')
ylabel('Packet Error rate')
grid
legend(ltext,'Location','NorthEast')

save('snr_error_mapping_variables.mat', '-v7.3')


