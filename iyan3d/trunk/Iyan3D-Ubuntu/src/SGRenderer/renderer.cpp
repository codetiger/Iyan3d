#include "common.h"

#define TILE_SIZE_X 8
#define TILE_SIZE_Y 8

void error_handler(const RTCError code, const char* str)
{
	printf("Embree: ");
	switch (code) {
		case RTC_UNKNOWN_ERROR    : printf("RTC_UNKNOWN_ERROR"); break;
		case RTC_INVALID_ARGUMENT : printf("RTC_INVALID_ARGUMENT"); break;
		case RTC_INVALID_OPERATION: printf("RTC_INVALID_OPERATION"); break;
		case RTC_OUT_OF_MEMORY    : printf("RTC_OUT_OF_MEMORY"); break;
		case RTC_UNSUPPORTED_CPU  : printf("RTC_UNSUPPORTED_CPU"); break;
		case RTC_CANCELLED        : printf("RTC_CANCELLED"); break;
		default                   : printf("invalid error code"); break;
	}
	if (str) { 
		printf(" ("); 
		while (*str) putchar(*str++); 
			printf(")\n"); 
	}
	exit(1);
}

string getFileContent(const char* file) {
	ifstream t(file);
	t.seekg(0, std::ios::end);
	size_t size = t.tellg();
	std::string machineId(size, ' ');
	t.seekg(0);
	t.read(&machineId[0], size); 
	return machineId;
}

TaskDetails getTaskFromServer(string machineId) {
	string url = "http://www.iyan3dapp.com/appapi/requesttask.php?machineid=" + machineId;
	downloadFile(url.c_str(), "taskid.txt");
	string taskInfo = getFileContent("taskid.txt");
	TaskDetails td;
	
	std::vector<std::string> x = split(taskInfo, ',');
	td.taskId = atoi(taskInfo.c_str());
	if(x.size() == 4) {
		td.taskId = atoi(x[0].c_str());
		td.frameCount = atoi(x[1].c_str());
		td.width = atoi(x[2].c_str());
		td.height = atoi(x[3].c_str());
	}

	return td;
}

bool downloadTaskFiles(TaskDetails td) {
	mkpath("data/", 0755);
	mkpath("data/" + to_string(td.taskId), 0755);
	printf("Downloading Task Files\n");
	string fileName = "data/" + to_string(td.taskId) + "/" + to_string(td.taskId) + ".zip";
	string url = "http://www.iyan3dapp.com/appapi/renderFiles/" + to_string(td.taskId) + ".zip";

	return downloadFile(url.c_str(), fileName.c_str());
}

void updateProgressToServer(int taskId, int frame) {
	string url = "http://www.iyan3dapp.com/appapi/updateprogress.php?taskid=" + to_string(taskId) + "&progress=" + to_string(frame);
	downloadFile(url.c_str(), "taskid.txt");
}

bool unzipTaskFiles(TaskDetails td) {
	chdir(("data/" + to_string(td.taskId)).c_str());
	printf("Extracting Task Files\n");

	string fileName = to_string(td.taskId) + ".zip";
	int err;
	struct zip *za = zip_open(fileName.c_str(), 0, &err);
	if(za == NULL) {
        printf("Can't open zip archive\n");
        return false;
	}

	for (int i = 0; i < zip_get_num_entries(za, 0); i++) {
		struct zip_stat sb;
        if (zip_stat_index(za, i, 0, &sb) == 0) {
            int len = strlen(sb.name);
            if (sb.name[len - 1] == '/') {
                printf("Content is in Directory\n");
            } else {
                struct zip_file *zf = zip_fopen_index(za, i, 0);
 
                int fd = open(sb.name, O_RDWR | O_TRUNC | O_CREAT, 0644);
 
                int sum = 0;
                while (sum != sb.size) {
                	char buf[100];
                    len = zip_fread(zf, buf, 100);
                    write(fd, buf, len);
                    sum += len;
                }
                close(fd);
                zip_fclose(zf);
            }
        }
    }   
 
    zip_close(za);
    return true;
}

bool renderFile(string fileName, TaskDetails td) {
	RTCDevice device = rtcNewDevice(NULL);
	rtcDeviceSetErrorFunction(device, error_handler);
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	Scene *scene = new Scene(device);
	bool status = scene->loadScene((fileName + ".sgfd").c_str(), td.width, td.height);
	if(status) {
		scene->render();
		scene->SaveToFile((fileName + "_render.png").c_str(), ImageFormat_PNG);
	}
	delete scene;
	rtcDeleteDevice(device);
}

bool uploadVideoToServer(TaskDetails td) {
	bool res = uploadFile(("http://www.iyan3dapp.com/appapi/finishtask.php?taskid=" + to_string(td.taskId)).c_str(), (to_string(td.taskId) + ".mpg").c_str());
}

bool renderTask(TaskDetails td) {
	if(downloadTaskFiles(td) && unzipTaskFiles(td)) {
		printf("Starting Render for Task %d\n", td.taskId);
		int frame = 1;
		string fileName = to_string(frame);
		struct stat buffer;   

		while(stat((fileName + ".sgfd").c_str(), &buffer) == 0) {
			printf("\nRendering Frame: %d\n", frame);
			renderFile(fileName, td);
			updateProgressToServer(td.taskId, frame);
			frame++;
			fileName = to_string(frame);
		}

		if(frame == 1) {
			printf("No SGFD Files in the Zip Archive\n");
			return false;
		}

		//video_encode(td.width, td.height, (to_string(td.taskId) + ".mpg").c_str());
		//uploadVideoToServer(td);
		//updateProgressToServer(td.taskId, frame);
		return true;
	}
	return false;
}

int main(int argc, char** argv) 
{
	printf("Starting Renderer\n");
	char cCurrentPath[FILENAME_MAX];
    getcwd(cCurrentPath, sizeof(cCurrentPath));
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';

	string machineId = getFileContent("config.cfg");
	printf("Working as Machine Id: %s\n", machineId.c_str());

	do {
		printf("Asking Server for new task\n");
		chdir(cCurrentPath);
		TaskDetails td = getTaskFromServer(machineId);

		if(td.taskId <= 0) {
			printf("No Pending Tasks waiting for %d seconds\n", taskFetchFrequency);
			sleep(taskFetchFrequency);
		} else {
			printf("Task Assigned: %d Frames: %d Width: %d Height: %d\n", td.taskId, td.frameCount, td.width, td.height);
			timespec ts;
			clock_gettime(CLOCK_REALTIME, &ts);

			if(renderTask(td)) {
				timespec te;
				clock_gettime(CLOCK_REALTIME, &te);
				double realtime = (te.tv_sec - ts.tv_sec) + (double)(te.tv_nsec - ts.tv_nsec) / (double)1000000000.0;
				printf("\nTime taken: %.2fs\n", realtime);
			}
		}
	} while(false);

	return 0;
}


