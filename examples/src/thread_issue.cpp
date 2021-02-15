

#include <iostream>
#include <cstdio>

#include "utility.hpp"

// Inludes common necessary includes for development using depthai library
#include "depthai/depthai.hpp"

void printSystemInformation(dai::SystemInformation);

void setupDevice(dai::Pipeline pipeline) {
    // @TODO modify device ctor to sleep after serialization
    // Connect to device
    dai::Device device(pipeline);

    // Create 'sysinfo' queue
    auto queue = device.getOutputQueue("sysinfo");

    // Query device (before pipeline starts)
    dai::MemoryInfo ddr = device.getDdrMemoryUsage();
    printf("Ddr used / total - %.2f / %.2f MiB\n", ddr.used / (1024.0f*1024.0f), ddr.total / (1024.0f*1024.0f));

    // Start pipeline 
    device.startPipeline();

    while(1){
        auto sysInfo = queue->get<dai::SystemInformation>();
        printSystemInformation(*sysInfo);
    }
}

int main(){
    using namespace std;

    dai::Pipeline pipeline;
    auto sysLog = pipeline.create<dai::node::SystemLogger>();
    auto xout = pipeline.create<dai::node::XLinkOut>();

    // properties
    sysLog->setRate(1.0f); // 1 hz updates
    xout->setStreamName("sysinfo");

    // links
    sysLog->out.link(xout->input);

    std::thread setup(setupDevice, pipeline);
    std::cout << "Main: " << pipeline.getNodeMap().size() << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // @TODO modify pipeline here, print stuff as well
    auto xout2 = pipeline.create<dai::node::XLinkOut>();
    xout2->setStreamName("sysinfo2");
    sysLog->out.link(xout2->input);
    std::cout << "Main: " << pipeline.getNodeMap().size() << "\n";
    setup.join();

    return 0;
}

void printSystemInformation(dai::SystemInformation info){

    printf("Ddr used / total - %.2f / %.2f MiB\n", info.ddrMemoryUsage.used / (1024.0f*1024.0f), info.ddrMemoryUsage.total / (1024.0f*1024.0f));
    printf("LeonCss heap used / total - %.2f / %.2f MiB\n", info.leonCssMemoryUsage.used / (1024.0f*1024.0f), info.leonCssMemoryUsage.total / (1024.0f*1024.0f));
    printf("LeonMss heap used / total - %.2f / %.2f MiB\n", info.leonMssMemoryUsage.used / (1024.0f*1024.0f), info.leonMssMemoryUsage.total / (1024.0f*1024.0f));
    const auto& t = info.chipTemperature;
    printf("Chip temperature - average: %.2f, css: %.2f, mss: %.2f, upa0: %.2f, upa1: %.2f\n", t.average, t.css, t.mss, t.upa, t.dss);
    printf("Cpu usage - Leon OS: %.2f %%, Leon RT: %.2f %%\n", info.leonCssCpuUsage.average * 100, info.leonMssCpuUsage.average * 100);
}
