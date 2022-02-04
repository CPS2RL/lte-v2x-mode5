clear
clc

P_keep  = 0:0.2:0.8; % Probability to keep the current resource
C_resel = [5 15];    % Range of reselection counter

n_sample = 10^6;     % # of samples for simulation
range = 0:200;       % Data bins for simulation

n = 1:10; % # of reselection to be considered for theoretical analysis

%% Simulation

data_sim  = zeros(length(P_keep),n_sample);
prob_sim = zeros(length(P_keep),length(range));

for pi=1:length(P_keep)
    
    for ni=1:n_sample
        flag_keep = 1;
        while flag_keep
%             data(pi,ni) = data(pi,ni) + randint*(C_resel(2)-C_resel(1))+C_resel(1);
            data_sim(pi,ni) = data_sim(pi,ni) + randi(C_resel,1);
            flag_keep = rand<P_keep(pi);
        end
    end
    
    prob_sim(pi,:) = histc(data_sim(pi,:),range)/n_sample;
    
    clc
    fprintf('----------------- Calculating -----------------------\n')
    fprintf('k/N = [ ');
    fprintf('%d/%d ', [pi; length(P_keep)]);
    fprintf(']\n');
end

fprintf('----------------- Done ---------------------\n')

%% Theoretical analysis

s = C_resel(2)-C_resel(1)+1; % max-min difference of reselection counter
p = 1:max(n)*s+max(n)*C_resel(1)-1; % Counter bins

prob_theo  = zeros(length(p),length(P_keep));
for pki=1:length(P_keep)
    
    data_theo = zeros(length(p),length(n));
    for ni=1:length(n)
        
        for pi=n(ni):n(ni)*s
            
            for k=0:floor((p(pi)-n(ni))/s)
                data_theo(pi+n(ni)*(C_resel(1)-1),ni) = data_theo(pi+n(ni)*(C_resel(1)-1),ni) + (-1)^k * nchoosek(n(ni),k) * nchoosek(p(pi)-s*k-1,n(ni)-1);
            end
            data_theo(pi+n(ni)*(C_resel(1)-1),ni) = (1-P_keep(pki))*P_keep(pki)^(n(ni)-1) * data_theo(pi+n(ni)*(C_resel(1)-1),ni)/(s^n(ni));
        end
        
    end
    prob_theo(:,pki) = sum(data_theo,2);
    
end


%% Plot
legend_cell = cell(length(P_keep),1);
for pi = 1:length(P_keep)
    legend_cell{pi} = sprintf('p_{keep} = %.1f (simulation)', P_keep(pi));
    legend_cell{pi+length(P_keep)} = sprintf('p_{keep} = %.1f (analytical)', P_keep(pi));
end

% PDF
figure(1)
plot(range,prob_sim,'-o');
hold on
plot(p,prob_theo,'--k');
xlabel('Number of transmissions between resource reselection')
ylabel('Probability P(x,p_{\rm keep})')
grid on
legend(legend_cell,'Location','NorthEast')
xlim([0 100])
saveas(gcf, 'resel_PDF', 'fig')
saveas(gcf, 'resel_PDF', 'png')

% CDF
figure(2)
plot(range,cumsum(prob_sim,2),'o-');
xlabel('Number of transmissions between resource reselection')
ylabel('CDF of P(x,p_{\rm keep})')
grid on
legend(legend_cell,'Location','SouthEast')
xlim([0 100])
ylim([0 1.01])
saveas(gcf, 'resel_CDF', 'fig')
saveas(gcf, 'resel_CDF', 'png')