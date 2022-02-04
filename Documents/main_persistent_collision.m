clear
clc

P_keep  = 0:0.2:0.8; % Probability to keep the current resource
C_resel = [5 15];    % Range of reselection counter

n_sample = 10^6;     % # of samples for simulation
range = 0:200;       % Data bins for simulation

n = 1:10; % # of reselection to be considered for theoretical analysis

%% Simulation

data_sim1  = zeros(length(P_keep),n_sample); % Node 1's # of transmission per resource reselection
data_sim2  = zeros(length(P_keep),n_sample); % Node 2's # of transmission per resource reselection
prob_sim = zeros(length(P_keep),length(range));

for pi=1:length(P_keep)
    
    for ni=1:n_sample
        flag_keep = 1;
        while flag_keep
%             data(pi,ni) = data(pi,ni) + randint*(C_resel(2)-C_resel(1))+C_resel(1);
            data_sim1(pi,ni) = data_sim1(pi,ni) + randi(C_resel,1);
            flag_keep = rand<P_keep(pi);
        end
    end
    
    for ni=1:n_sample
        flag_keep = 1;
        while flag_keep
%             data(pi,ni) = data(pi,ni) + randint*(C_resel(2)-C_resel(1))+C_resel(1);
            data_sim2(pi,ni) = data_sim2(pi,ni) + randi(C_resel,1);
            flag_keep = rand<P_keep(pi);
        end
    end
    
    prob_sim(pi,:) = histc(min(data_sim1(pi,:),data_sim2(pi,:)),range)/n_sample;
    
    clc
    fprintf('----------------- Calculating -----------------------\n')
    fprintf('k/N = [ ');
    fprintf('%d/%d ', [pi; length(P_keep)]);
    fprintf(']\n');
end

data_sim_mean = mean(min(data_sim1,data_sim2),2);

fprintf('----------------- Done ---------------------\n')

%% Theoretical analysis


s = C_resel(2)-C_resel(1)+1; % max-min difference of reselection counter
p = 1:max(n)*s+max(n)*C_resel(1)-1; % Counter bins

prob_tc_theo = zeros(length(p),length(P_keep)); % Probability of each # of transmission per resource reselection
for pki=1:length(P_keep)
    
    data_tc_theo = zeros(length(p),length(n));
    for ni=1:length(n)
        
        for pi=n(ni):n(ni)*s
            
            for k=0:floor((p(pi)-n(ni))/s)
                data_tc_theo(pi+n(ni)*(C_resel(1)-1),ni) = data_tc_theo(pi+n(ni)*(C_resel(1)-1),ni) + (-1)^k * nchoosek(n(ni),k) * nchoosek(p(pi)-s*k-1,n(ni)-1);
            end
            data_tc_theo(pi+n(ni)*(C_resel(1)-1),ni) = (1-P_keep(pki))*P_keep(pki)^(n(ni)-1) * data_tc_theo(pi+n(ni)*(C_resel(1)-1),ni)/(s^n(ni));
        end
        
    end
    prob_tc_theo(:,pki) = sum(data_tc_theo,2);
    
end

prob_pc_theo = zeros(length(p),length(P_keep)); % Probability of each # of persistent collisions
for pki=1:length(P_keep)
    
    data_pc_theo = zeros(length(p),length(n));
    for pi=p(1):p(end)
        prob_pc_theo(pi,pki) = prob_tc_theo(pi,pki)^2 + 2*prob_tc_theo(pi,pki)*(1-sum(prob_tc_theo(1:p(pi),pki)));
    end
end

%% Plot
legend_cell = cell(length(P_keep),1);
for pi = 1:length(P_keep)
    legend_cell{pi} = sprintf('p_{keep} = %.1f (simulation)', P_keep(pi));
    legend_cell{pi+length(P_keep)} = sprintf('p_{keep} = %.1f (analytical)', P_keep(pi));
end

figure(1)
plot(range,prob_sim,'-o');
hold on
plot(p,prob_pc_theo,'--k');
xlabel('Number of persistent collisions between two nodes')
ylabel('Probability Pr(y)')
grid on
legend(legend_cell,'Location','NorthEast')
xlim([0 100])
saveas(gcf, 'percol_PDF', 'fig')
saveas(gcf, 'percol_PDF', 'png')


% CDF
figure(2)
plot(range,cumsum(prob_sim,2),'o-');
xlabel('Number of persistent collisions between two nodes')
ylabel('CDF of Pr(y)')
grid on
legend(legend_cell,'Location','SouthEast')
xlim([0 100])
ylim([0 1.01])
saveas(gcf, 'percol_CDF', 'fig')
saveas(gcf, 'percol_CDF', 'png')