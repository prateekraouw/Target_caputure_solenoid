#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::ofstream testFile("test_6d_vector.csv");
    if (testFile.is_open()) {
        testFile << "Detector,ParticleType,x[cm],px[MeV/c],y[cm],py[MeV/c],z[cm],pz[MeV/c],TotalEnergy[MeV]" << std::endl;
        testFile << "1,mu+,10.5,100.0,5.2,50.0,100.0,200.0,105.0" << std::endl;
        testFile << "2,pi-,15.3,150.0,8.1,80.0,200.0,300.0,155.0" << std::endl;
        testFile.close();
        std::cout << "Test 6D vector file created successfully!" << std::endl;
    } else {
        std::cout << "Failed to create test file!" << std::endl;
    }
    return 0;
} 