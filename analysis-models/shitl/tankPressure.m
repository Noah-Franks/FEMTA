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
    -> MUST VERIFY PERFECT DIAPHRAGM ASSUMPTION!
@author: Justin C (pressure build-up in propellant tank)
@co-author: Alan J (pipe flows (not yet implemented here))
%}

clear
LVTF_ambientP = load('LVTF_ambientP.mat').ans;
livePlot = 0; %bool plot data live (LAGGY!!)
stoData = 1; %bool store data in arrays at every time step

%initial conditions
ctrlChmbrVent = 0;
ctrlTankVent = 0;
ambientP = 45000;
chmbrTG = 297;
chmbrTL = 298.3;
chmbrPNvc = nvcVP(chmbrTL);
chmbrPAir = 45000-chmbrPNvc;
chmbrML = 0.1*(1e-6)*nvcRho(chmbrTL);
chmbrVL = chmbrML/nvcRho(chmbrTL);
chmbrVG = (5e-7)-chmbrVL;
chmbrSAL = 0.0000149034;
chmbrMGAir = chmbrPAir*chmbrVG/287.05/chmbrTG;
chmbrMGNvc = chmbrPNvc*chmbrVG/33.25/chmbrTG;
chmbrVentDia = 0.007;
tankTG = 297;
tankTL = 297;
tankPH2o = h2oVP(tankTL);
tankPAir = ambientP-tankPH2o;
tankVL = 14*(1e-6);
tankVG = 0;
tankLRho = 997; %initial density
tankML = tankLRho*tankVL;
tankSAL = 0;
tankMGAir = tankPAir*tankVG/287.05/tankTG;
tankMGH2o = tankPH2o*tankVG/461.52/tankTG;
tankVentDia = 0.007;


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
    simData.physics.volume = zeros([0,0]);
end

dt = 0.03;
ff = 0.001;
for simTime = 0:dt:150
    if(simTime >= 23.7)
        ctrlChmbrVent = 1;
        %
        ctrlTankVent = 1;
        if(simTime >= 60)
            ctrlChmbrVent = 0;
            if(simTime >= 90)
                ctrlChmbrVent = 1;
            end
        end
        %
    end
    ambientP = 1000*LVTF_ambientP(find(LVTF_ambientP(:,1) > simTime, 1, 'first'), 2);
    %%
    nvcEC = 0.006; %0.01625
    nvcCC = 0.00606; %0.016314
    if(ctrlChmbrVent == 0)
        nvcEC = 0.0115;
        nvcCC = 0.009;
    end
    %%
    mDot = nvcEvap(nvcEC, nvcCC, chmbrSAL, chmbrPNvc, chmbrTG, chmbrTL);
    chmbrML = chmbrML - mDot*dt;
    %fprintf("%0.10f kg\t", chmbrML);
    if(chmbrML < 0)
        mDot = mDot - abs(chmbrML)/dt;
        chmbrML = 0;
    end
    chmbrVG = chmbrVG - (chmbrML/nvcRho(chmbrTL)-chmbrVL);
    chmbrVL = chmbrVL + (chmbrML/nvcRho(chmbrTL)-chmbrVL);
    chmbrMGNvc = chmbrMGNvc + mDot*dt;
    chmbrPNvc = chmbrMGNvc*33.25*chmbrTG/chmbrVG;
    %%
    %{
    mDot = nvcEvap(1.0, 1.0, tankSAL, chmbrPNvc, chmbrTG, chmbrTL);
    tankML = tankML - mDot*dt;
    if(tankML < 0)
        mDot = mDot - abs(tankML)/dt;
        tankML = 0;
    end
    tankMGH2o = tankMGH2o + mDot*dt;
    tankPH2o = tankMGH2o*461.52*tankTG/tankVG;
    %}
    %%
    if(chmbrML > 0)
        if(ctrlChmbrVent == 0)
            dT = (112/1000) * -mDot * chmbrSAL * (1/(1.183*chmbrML));
            chmbrTG = chmbrTG + 2700000*dT*dt;
            dT = 1.1e8*dT;
            dT = dT - (chmbrML/0.0015)*50*(1*chmbrSAL*(chmbrTL-chmbrTG)*(1/(1.183*chmbrML)));
            chmbrTL = chmbrTL + dT*dt;
        else
            dT = (2.47e10*chmbrML/0.0015)*(112/1000) * mDot * chmbrSAL * (1/(1.183*chmbrML));
            dT = dT - 0.024*(chmbrSAL*((chmbrTL+1)-chmbrTG)*(1/(1.183*chmbrML)));
            chmbrTL = chmbrTL + dT*dt;
            dT = -0.03*(1*chmbrSAL*(chmbrTG-(chmbrTL+1))*(1/(1.183*chmbrML)));
            chmbrTG = chmbrTG + dT*dt;
        end
    else
        chmbrML = 0;
        chmbrTL = chmbrTG;
    end
    %fprintf("%0.10f K\n", chmbrTG)
    %%
    if(ctrlChmbrVent == 0)
        mDot = ff*mDotThruOrifice(ambientP, chmbrPAir+chmbrPNvc, (chmbrMGAir+chmbrMGNvc)/chmbrVG, (1.4*chmbrMGAir+1.0289*chmbrMGNvc)/(chmbrMGAir+chmbrMGNvc), 0.8, chmbrVentDia);
        chmbrMGAir = chmbrMGAir + mDot*(chmbrMGAir/(chmbrMGAir+chmbrMGNvc))*dt;
        chmbrMGNvc = chmbrMGNvc + mDot*(chmbrMGNvc/(chmbrMGAir+chmbrMGNvc))*dt;
        if(chmbrMGAir < 0)
            chmbrMGAir = 0;
        end
        if(chmbrMGNvc < 0)
            chmbrMGNvc = 0;
        end
        chmbrPAir = chmbrMGAir*287.05*chmbrTG/chmbrVG;
        chmbrPNvc = chmbrMGNvc*33.25*chmbrTG/chmbrVG;
    end
    %{
    if(ctrlTankVent == 0)
        if(tankSAL == 0)
            tankSAL = pi*(tankVentDia/2)^2;
        end
        mDot = ff*mDotThruOrifice(ambientP, tankPAir+tankPH2o, (tankMGAir+tankMGH2o)/tankV, (1.4*tankMGAir+1.329*tankMGH2o)/(tankMGAir+tankMGH2o), 0.8, tankVentDia);
        tankMGAir = tankMGAir + mDot*(tankMGAir/(tankMGAir+tankMGH2o))*dt;
        tankMGH2o = tankMGH2o + mDot*(tankrMGH2o/(tankMGAir+tankMGH2o))*dt;
        if(tankMGAir < 0)
            tankMGAir = 0;
        end
        if(tankMGH2o < 0)
            tankMGH2o = 0;
        end
        tankPAir = tankMGAir*287.05*tankTG/tankV;
        tankPH2o = tankMGH2o*461.52*tankTG/tankV;
    end
    %}
    %%
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
        newMRow = [chmbrML, chmbrMGAir, chmbrMGNvc, tankML, tankMGAir, tankMGH2o];
        newPRow = [ambientP/1000, (chmbrPNvc+chmbrPAir)/1000, nvcVP(chmbrTL)/1000, (tankPH2o+tankPAir)/1000, h2oVP(tankTL)/1000];
        newTRow = [chmbrTG, chmbrTL, tankTG, tankTL];
        newVRow = [chmbrVG, chmbrVL];
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
            if size(simData.physics.volume, 2) ~= size(newVRow, 2)
                simData.physics.volume = zeros([0, size(newVRow, 2)]);
            end
        end
        simData.physics.mass = [simData.physics.mass; newMRow];
        simData.physics.pressure = [simData.physics.pressure; newPRow];
        simData.physics.temperature = [simData.physics.temperature; newTRow];
        simData.physics.volume = [simData.physics.volume; newVRow];
    end
    fprintf("%0.2f s\n", simTime)
    
end
hold off;

plot(simData.time, simData.physics.pressure)
ylim([-5 45])


%Diaphragm dP characteristic
function frontP = pDiaph(backP)
    frontP = backP; %perfect diaphragm assumption MUST VERIFY!!! DO NOT USE FOR FINAL
end
%Evaporation characteristic of H2O (surfArea in m^3, gasP in Pa, gasT in K, fluid T in K)
function mDot = h2oEvap(sigE, sigC, surfArea, gasP, gasT, fluidT) %a positive (+) mDot signifies mass leaving liquid (evaporating)
    mDot = surfArea * sqrt((4.1528e-25)/(2*pi*physconst('boltzmann')))*(sigE*nvcVP(fluidT)/sqrt(fluidT) - sigC*gasP/sqrt(gasT));
end
%Vapor Pressure of H2O (T in K, vp in Pa)
function vp = h2oVP(T)
    if(T >= 255.9 && T < 273)
        A = 4.6543;
        B = 1435.264;
        C = -64.848;
    elseif(T >= 273 && T <= 303.5)
        A = 5.40221;
        B = 1838.675;
        C = -31.737;
    elseif(T > 303.5 && T <= 333.5)
        A = 5.20389;
        B = 1733.926;
        C = -39.485;
    elseif(T > 333.5 && T <= 363)
        A = 5.0768;
        B = 1659.793;
        C = -45.854;
    end
    vp = 100000 * 10^(A-(B/(T+C)));
end
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
