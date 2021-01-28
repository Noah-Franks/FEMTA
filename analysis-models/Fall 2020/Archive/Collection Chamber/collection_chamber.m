clear
clc
%Global Definitions
global m kB rhol N_a MW D A R C_evap C_cond V_CC h_evap Cp h_c;        
m=2.988e-26;        %mass of one water molecule [kg]       
kB=1.380649e-23;    %Boltzmann constant [J/K]      
rhol=997;           %density of liquid water [kg/m^3]    
N_a=6.02214e23;     %Avogadros number [# of particles/mol]
MW=18.0135;         %Molecular weight of water [kg/kmol]
D=(1/8)/39.37;      %Pipe diameter [m]
A=pi*((D/2)^2)*33;  %Area of pipe cross section [m^2]
R=8.314;            %Universal Gas Constant of Water [kJ/kmol-K]
C_evap=1;           %Evaporation Coefficient
C_cond=0.75;        %Condensation Coefficient
V_CC=38e-6;         %volume of CC [m^3]
h_evap=2260;        %heat of vaporazitaion of water [kJ/kg]
Cp=4;               %specific heat of water vapor [kJ/kg-K]
h_c=400;           %convective heat transfer coefficient of water [kW/m^2-K]

%Iteration Starters
dt=0.03;            %timestep [s]
Pg=0.00001;         %initial Pressure in CC
dT=300;             %initial temp [K]
m_liquid=0;         %initial mass of liquid water in CC
m_vapor=0;          %initial mass of water vapor in CC
fluid_density=0;    %initial density of fluid in CC
totalTime=150;      %duration of experiment

%Storage Vectors
N=round(totalTime/dt)+1;
count=1;
CCPressure=zeros(N,1);
massLiquid_array=zeros(N,1);
massVapor_array=zeros(N,1);
volumeVapor_array=zeros(N,1);
HerKnu_array=zeros(N,1);
orifice_array=zeros(N,1);
temp_array=zeros(N,1);
Pv_array=zeros(N,1);
Q_array=zeros(N,1);

%ANTOINE EQUATION CONSTANTS (valid for 274-373 K) (generates P in mmHg)
Z=8.07131;
B=1730.63;
C=233.426;
%ANTOINE EQUATION CONSTANTS (valid for 374-647 K) (generates P in mmHg)
E=8.14019;
F=1810.94;
G=244.485;

function m_transfer=HerKnu(Ps,T_liquid,T_vapor,Pg,m,A,C_evap,C_cond)
    global kB;
    m_transfer=A*(sqrt(m/(2*pi*kB))*((C_evap*(Ps/sqrt(T_vapor)))-(C_cond*(Pg/sqrt(T_liquid)))));
end

%Iterative Loop to determine pressure for experiment duration
for simTime=0:dt:totalTime
        
    %Vapor Pressure of Water
    if dT<=373
        Pv=10.^(Z-(B./(C+(dT-273))));        %interface pressure in mmHg
        Pv=Pv*133;                           %conversion to Pa from mmHg
    elseif dT>374
        Pv=10.^(E-(F./(G+(dT-273))));        %interface pressure in mmHg
        Pv=Pv*133;                           %conversion to Pa from mmHg
    end
    
    Pv_array(count)=Pv/1000;
    
    %mass of liquid water in CC [kg]
    mdot_liquid=((14/totalTime)/(1e6))*rhol;     %Assuming constant linear mass flow
    m_liquid=(mdot_liquid*dt)+m_liquid;
    
    %mass of water vapor in CC [kg]
    m_vapor=(HerKnu(Pv,dT,Pg)*dt)+m_vapor;
    m_liquid=m_liquid-(HerKnu(Pv,dT,Pg)*dt);
    massVapor_array(count)=m_vapor;
    HerKnu_array(count)=HerKnu(Pv,dT,Pg);
    massLiquid_array(count)=m_liquid;
    
    %volume of liquid in CC [m^3]
    V_liquid=(m_liquid/rhol);
    
    %volume of gas in CC [m^3]
    V_vapor=V_CC-V_liquid;
    volumeVapor_array(count)=V_vapor;
    fluid_density=m_vapor/V_vapor;
    
    %Pressure in CC at dt [Pa]
    Pg=(m_vapor*R*dT)/(MW*V_vapor);
    
    %Temperature Change due to Evaportion
    temp_array(count)=dT;
    dT=((h_evap*-HerKnu(Pv,dT,Pg)*dt)/(m_liquid*Cp))+dT;
    %dT=((h_evap*HerKnu(Pv,dT,Pg)*dt)/(h_c*A*dt))+dT;
    
    Q_array(count)=h_evap*HerKnu(Pv,dT,Pg)*dt;
    
    %Storage Vector Fill
    CCPressure(count)=Pg/1000;
    count=count+1; 
end

%Plot of Collection Chamber Pressure
figure(1)
plot((0:dt:totalTime),CCPressure,'Linewidth',3)
hold on
plot((0:dt:totalTime),zeros(length(0:dt:totalTime),1)+Pv/1000,'Linewidth',1)
title("Collection Chamber Pressure for Duration of Experiment",'Fontsize',22)
ylabel("Pressure [kPa]",'Fontsize',17)
xlabel("Time [s]",'Fontsize',17)
legend('Pressure in Collection Chamber [Pa]','Final Vapor Pressure [Pa]')
grid on

%Mass transfer from gas to liquid [kg/s]
%Negative denotes vapor to liquid (condensation), Positive denotes liquid to vapor
%(evaporation)
function m_transfer=HerKnu(Pv,dT,Pg)
    global m kB A C_evap C_cond;
    m_transfer=A*(sqrt(m/(2*pi*kB))*((C_evap*(Pv/sqrt(dT)))-(C_cond*(Pg/sqrt(dT)))));
end

% figure(2)
% plot((0:dt:totalTime),temp_array)
% title("Temperature for Duration of Experiment")
% 
% figure(3)
% plot((0:dt:totalTime),volumeVapor_array)
% title("Volume of Vapor for Duration of Experiment")
% 
% figure(4)
% plot((0:dt:totalTime),HerKnu_array)
% title("Mass Transfer for Duration of Experiment")