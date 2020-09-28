%{
Created on Fri Sept 22 22:34:15 2020
    Simulated Hardware In The Loop (SHITL) code for the virtual
    environment. The virtual environment's purpose is to test the flight
    computer to ensure proper (and predictable) functionality. This will
    suplement full system testing once all hardware is manufactured and in
    a way serves as a precursor test to work out any issues while hardware
    is being manufactured.
    -> This code take in inputs for valve actuations and outputs all flow
    data throughout the system. Values are most notably pressure and
    temperature, however, other non-measureable values are calculated such
    as masses and partial pressures.
@author: Justin C (pressure build-up in propellant tank)
@co-author: Alan J (pipe flows (not yet implemented here))
%}

clear
LVTF_ambientP = load('LVTF_ambientP.mat').ans;
livePlot = 0; %bool plot data live (LAGGY!!)
stoData = 1; %bool store data in arrays at every time step

%initial conditions
ctrlVent = 0;
chmbrTG = 297;
chmbrTL = 298.3;
chmbrPNvc = nvcVP(chmbrTL);
chmbrPAir = 45000-chmbrPNvc;
ambientP = 45000;
chmbrV = 5e-7;
chmbrML = 1*(1e-6)*nvcRho(chmbrTL);
chmbrSAL = 0.0000149034;
chmbrMGAir = chmbrPAir*chmbrV/287.05/chmbrTG;
chmbrMGNvc = chmbrPNvc*chmbrV/33.25/chmbrTG;
ventDia = 0.007;

if livePlot
    clf;
    hold on;
    yyaxis left;
    xlim([0 150]);
    ylim([-5 45]);
    yyaxis right;
    ylim([282 300]);
    grid on;
end
if stoData
    simData.time = zeros([0,0]);
    simData.physics.mass = zeros([0,0]);
    simData.physics.pressure = zeros([0,0]);
    simData.physics.temperature = zeros([0,0]);
end

dt = 0.03;
ff = 0.001;
for simTime = 0:dt:150
    if(simTime >= 23.7)
        ctrlVent = 1;
        if(simTime >= 60)
            ctrlVent = 0;
            if(simTime >= 90)
                ctrlVent = 1;
            end
        end
    end
    ambientP = 1000*LVTF_ambientP(find(LVTF_ambientP(:,1) > simTime, 1, 'first'), 2);
    
    nvcEC = 0.006; %0.01625
    nvcCC = 0.0061; %0.016314
    if(ctrlVent == 0)
        nvcEC = 0.0115;
        nvcCC = 0.009;
    end
    
    mDot = nvcEvap(nvcEC, nvcCC, chmbrSAL, chmbrPNvc, chmbrTG, chmbrTL);
    chmbrML = chmbrML - mDot*dt;
    %fprintf("%0.10f kg\t", chmbrML);
    if(chmbrML < 0)
        mDot = mDot - abs(chmbrML)/dt;
        chmbrML = 0;
    end
    chmbrMGNvc = chmbrMGNvc + mDot*dt;
    chmbrPNvc = chmbrMGNvc*33.25*chmbrTG/chmbrV;
    
    if(chmbrML > 0)
        if(ctrlVent == 0)
            dT = (112/1000) * -mDot * chmbrSAL * (1/(1.183*chmbrML));
            chmbrTG = chmbrTG + 27000000*dT*dt;
            dT = 1.4e9*dT;
            dT = dT - (chmbrML/0.0015)*50*(1*chmbrSAL*(chmbrTL-chmbrTG)*(1/(1.183*chmbrML)));
            chmbrTL = chmbrTL + dT*dt;
        else
            dT = (2.1e10*chmbrML/0.0015)*(112/1000) * mDot * chmbrSAL * (1/(1.183*chmbrML));
            dT = dT - 0.3*(1*chmbrSAL*((chmbrTL+1)-chmbrTG)*(1/(1.183*chmbrML)));
            chmbrTL = chmbrTL + dT*dt;
            dT = -0.3*(1*chmbrSAL*(chmbrTG-(chmbrTL+1))*(1/(1.183*chmbrML)));
            chmbrTG = chmbrTG + dT*dt;
        end
    else
        chmbrTL = 0;
    end
    %fprintf("%0.10f K\n", chmbrTG)
    
    if(ctrlVent == 0)
        mDot = ff*mDotThruOrifice(ambientP, chmbrPAir+chmbrPNvc, (chmbrMGAir+chmbrMGNvc)/chmbrV, (1.4*chmbrMGAir+1.0289*chmbrMGNvc)/(chmbrMGAir+chmbrMGNvc), 0.8, ventDia);
        chmbrMGAir = chmbrMGAir + mDot*(chmbrMGAir/(chmbrMGAir+chmbrMGNvc))*dt;
        chmbrMGNvc = chmbrMGNvc + mDot*(chmbrMGNvc/(chmbrMGAir+chmbrMGNvc))*dt;
        if(chmbrMGAir < 0)
            chmbrMGAir = 0;
        end
        if(chmbrMGNvc < 0)
            chmbrMGNvc = 0;
        end
        chmbrPAir = chmbrMGAir*287.05*chmbrTG/chmbrV;
        chmbrPNvc = chmbrMGNvc*33.25*chmbrTG/chmbrV;
    end
    
    if livePlot
        yyaxis left
        plot(simTime, (chmbrPAir+chmbrPNvc)/1000, '.b', 'MarkerSize', 2);
        plot(simTime, ambientP/1000, '.r', 'MarkerSize', 2);
        plot(simTime, nvcVP(chmbrTL)/1000, '.c', 'MarkerSize', 1);
        yyaxis right
        plot(simTime, chmbrTL, '.g', 'MarkerSize', 2);
        plot(simTime, chmbrTG, '.y', 'MarkerSize', 2);
        drawnow;
    end
    if stoData
        simData.time = [simData.time; simTime];
        newMRow = [chmbrML, chmbrMGAir, chmbrMGNvc];
        newPRow = [ambientP/1000, (chmbrPNvc+chmbrPAir)/1000, nvcVP(chmbrTL)/1000];
        newTRow = [chmbrTG, chmbrTL];
        if simTime == 0
            if size(simData.physics.mass, 2) ~= size(newMRow, 2)
                simData.physics.mass = zeros([0, size(newMRow, 2)]);
            end
            if size(simData.physics.pressure, 2) ~= size(newPRow, 2)
                simData.physics.pressure = zeros([0, size(newPRow, 2)]);
            end
            if size(simData.physics.temperature, 2) ~= size(newTRow, 2)
                simData.physics.temperature = zeros([0, size(newTRow, 2)]);
            end
        end
        simData.physics.mass = [simData.physics.mass; newMRow];
        simData.physics.pressure = [simData.physics.pressure; newPRow];
        simData.physics.temperature = [simData.physics.temperature; newTRow];
    end
    fprintf("%0.2f s\n", simTime)
    
end
hold off;






%Evaporation characteristic of NV 7100 (surfArea in m^3, gasP in Pa, gasT in K, fluid T in K)
function mDot = nvcEvap(sigE, sigC, surfArea, gasP, gasT, fluidT) %a positive (+) mDot signifies mass leaving liquid (evaporating)
    mDot = surfArea * sqrt((4.1528e-25)/(2*pi*physconst('boltzmann')))*(sigE*nvcVP(fluidT)/sqrt(fluidT) - sigC*gasP/sqrt(gasT));
end
%Vapor Pressure of NV 7100 (T in K, vp in Pa)
function vp = nvcVP(T)
    vp = exp(22.415 - 3641.9 * (1/T));
end
%Density of NV 7100 Liquid (T in K)
function dD = nvcRho(T)
    dD = 1.5383 - 0.002269*(T-273.15);
    dD = dD / (1000 * 0.000001); %kg/m^3
end
