#include "common.h"
#include "HeaderFiles/SGEditorScene.h"
#include "HeaderFiles/json_json.h"

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
	if(t) {
		t.seekg(0, std::ios::end);
		size_t size = t.tellg();
		std::string machineId(size, ' ');
		t.seekg(0);
		t.read(&machineId[0], size);
		return machineId;
	}
	return "";
}

TaskDetails getRenderTaskFromServer(string machineId) {
	string url = "https://www.iyan3dapp.com/appapi/requesttask.php?machineid=" + machineId;
	downloadFile(url.c_str(), "taskid.txt");
	string taskInfo = getFileContent("taskid.txt");
	TaskDetails td;
	td.taskId = -1;
	td.isRenderTask = true;
	
	if(taskInfo.size() > 0) {
		std::vector<std::string> x = split(taskInfo, ',');
		td.taskId = atoi(taskInfo.c_str());
		if(x.size() == 4) {
			td.taskId = atoi(x[0].c_str());
			td.frame = atoi(x[1].c_str());
			td.width = atoi(x[2].c_str());
			td.height = atoi(x[3].c_str());
		}
	}

	return td;
}

TaskDetails getVideoTaskFromServer(string machineId) {
	string url = "https://www.iyan3dapp.com/appapi/videotask.php?machineid=" + machineId;
	downloadFile(url.c_str(), "taskid.txt");
	string taskInfo = getFileContent("taskid.txt");
	TaskDetails td;
	td.taskId = -1;
	td.isRenderTask = false;

	if(taskInfo.size() > 0) {
		std::vector<std::string> x = split(taskInfo, ',');
		td.taskId = atoi(taskInfo.c_str());
		if(x.size() == 5) {
			td.taskId = atoi(x[0].c_str());
			td.startFrame = atoi(x[1].c_str());
			td.endFrame = atoi(x[2].c_str());
			td.width = atoi(x[3].c_str());
			td.height = atoi(x[4].c_str());
		}
	}

	return td;
}

TaskDetails getTaskFromServer(string machineId) {
	TaskDetails td;
	if(isRenderMachine) {
		td = getRenderTaskFromServer(machineId);
		if(td.taskId <= 0)
			td = getVideoTaskFromServer(machineId);
	} else {
		td = getVideoTaskFromServer(machineId);
		if(td.taskId <= 0)
			td = getRenderTaskFromServer(machineId);
	}
	return td;
}

bool downloadTaskFiles(TaskDetails td) {
	mkpath("data/", 0755);
	mkpath("data/" + to_string(td.taskId), 0755);
	printf("Downloading Task Files\n");
	string fileName = "data/" + to_string(td.taskId) + "/" + to_string(td.taskId) + ".zip";
	string url = "https://www.iyan3dapp.com/appapi/renderFiles/" + to_string(td.taskId) + ".zip";

	if(file_exists(fileName)) {
		return true;
	} else
		return downloadFile(url.c_str(), fileName.c_str());
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

bool renderFile(TaskDetails td) {
	RTCDevice device = rtcNewDevice(NULL);
	rtcDeviceSetErrorFunction(device, error_handler);
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	Scene *scene = new Scene(device);
	bool status = scene->loadScene((to_string(td.frame) + ".sgfd").c_str(), td.width, td.height);
	if(status) {
		scene->render();
		scene->SaveToFile((convert2String(td.taskId) + "t" + convert2String(td.frame) + "f_render.png").c_str(), ImageFormat_PNG);
	}
	delete scene;
	rtcDeleteDevice(device);
	return status;
}

bool uploadOutputToServer(TaskDetails td) {
	string filename = convert2String(td.taskId) + "t" + convert2String(td.frame) + "f_render.png";
	return uploadFile(("https://www.iyan3dapp.com/appapi/finishtask.php?taskid=" + to_string(td.taskId) + "&frame=" + to_string(td.frame)).c_str(), filename.c_str());
}

bool checkAndDownloadFile(std::string filePath, std::string serverPath, std::string folder) {
	if(file_exists(filePath))
		return true;

	string commonFilePath = "../" + filePath;
	if(!file_exists(commonFilePath)) {
		string url = "https://www.iyan3dapp.com/appapi" + folder + "/" + serverPath;
		printf("Fetching Url: %s\n", url.c_str());
		downloadFile(url.c_str(), commonFilePath.c_str());
	}
	file_copy(commonFilePath, filePath);

	if(!file_exists(filePath))
		return false;

	return true;
}

bool downloadMissingAssetCallBack(std::string filePath, NODE_TYPE nodeType, bool hasTexture) {
	if(nodeType == NODE_SGM) {
		if(!checkAndDownloadFile(filePath + ".sgm", filePath + ".sgm", "/mesh"))
			return false;
		if(hasTexture && !checkAndDownloadFile(filePath + "-cm.png", filePath + ".png", "/meshtexture"))
			return false;
	} else if(nodeType == NODE_RIG) {
		if(!checkAndDownloadFile(filePath + ".sgr", filePath + ".sgr", "/mesh"))
			return false;
		if(hasTexture && !checkAndDownloadFile(filePath + "-cm.png", filePath + ".png", "/meshtexture"))
			return false;
	} else if(nodeType == NODE_OBJ) {
		if(!checkAndDownloadFile(filePath + ".obj", filePath + ".obj", ""))
			return false;
		if(hasTexture && !checkAndDownloadFile(filePath + "-cm.png", filePath + ".png", ""))
			return false;
	} else if(nodeType == NODE_TEXT) {
		string uefilename(url_encode(filePath.c_str()));
		if(!checkAndDownloadFile(filePath, "findfont.php?name=" + uefilename, ""))
			return false;
	}

	return true;
}

bool downloadVideoTaskFiles(TaskDetails td) {
	mkpath("data/", 0755);
	mkpath("data/video/", 0755);
	mkpath("data/video/" + to_string(td.taskId), 0755);
	printf("Downloading Video Task Files\n");

	printf("Start: %d End: %d\n", td.startFrame, td.endFrame);
	for(int i = td.startFrame; i <= td.endFrame; i++) {
		printf("Downloading Frame: %d\n", i);
		string fileName = "data/video/" + to_string(td.taskId) + "/" + convert2String(td.taskId) + "t" + convert2String(i) + "f_render.png";
		string url = "https://www.iyan3dapp.com/appapi/renderFiles/"  + to_string(td.taskId) + "/" + convert2String(td.taskId) + "t" + convert2String(i) + "f_render.png";
		if(!file_exists(fileName))
			if(!downloadFile(url.c_str(), fileName.c_str()))
				return false;
	}
	return true;
}

string exec(const char* cmd) {
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

bool uploadVideoToServer(TaskDetails td) {
	string filename = to_string(td.taskId) + ".mp4";
	return uploadFile(("https://www.iyan3dapp.com/appapi/finishtask.php?taskid=" + to_string(td.taskId) + "&frame=0").c_str(), filename.c_str());
}

bool videoTask(TaskDetails td) {
	if(!downloadVideoTaskFiles(td))
		return false;

	printf("Creating Video File\n");

	chdir(("data/video/" + to_string(td.taskId)).c_str());

	string cmd = "convert -delay 1 " + convert2String(td.taskId) + "t*f_render.png " + to_string(td.taskId) + ".mp4";
	exec(cmd.c_str());

	uploadVideoToServer(td);
	return true;
}

bool renderTask(TaskDetails td) {
	if(!downloadTaskFiles(td))
		return false;

	if(!unzipTaskFiles(td))
		return false;

	printf("Creating SGFD Files\n");
	constants::BundlePath = ".";
	checkAndDownloadFile("camera.sgm", "camera.sgm", "/mesh");
	checkAndDownloadFile("light.sgm", "light.sgm", "/mesh");
	checkAndDownloadFile("sphere.sgm", "sphere.sgm", "/mesh");

	SceneManager *smgr = new SceneManager(td.width, td.height, 1.0, OPENGLES2, "", NULL);
	SGEditorScene *scene = new SGEditorScene(OPENGLES2, smgr, td.width, td.height);
	scene->downloadMissingAssetCallBack = &downloadMissingAssetCallBack;

	std::string filename = "index.sgb";
	scene->loadSceneData(&filename);
	scene->generateSGFDFile(td.frame);

	printf("Starting Render for Task %d\n", td.taskId);
	struct stat buffer;

	if(stat((to_string(td.frame) + ".sgfd").c_str(), &buffer) == 0) {
		printf("Rendering Frame: %d\n", td.frame);
		renderFile(td);
	} else {
		printf("No SGFD Files in the Zip Archive\n");
		return false;
	}

	uploadOutputToServer(td);
	return true;
}

int main(int argc, char** argv) 
{
	char cCurrentPath[FILENAME_MAX];
    getcwd(cCurrentPath, sizeof(cCurrentPath));
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
    std::setlocale (LC_ALL, "");

    string machineId;
    Json::Value configData;
    Json::Reader reader;
    ifstream jsonFile("config.cfg");

    if(reader.parse(jsonFile, configData)) {
		machineId = configData["machineId"].asString();
		isRenderMachine = configData["isRenderMachine"].asBool();
		taskFetchFrequency = configData["taskFetchFrequency"].asInt();
		MAX_RAY_DEPTH = configData["MAX_RAY_DEPTH"].asInt();
		samplesAO = configData["samplesAO"].asInt();
		minAOBrightness = configData["minAOBrightness"].asDouble();
		antiAliasingSamples = configData["antiAliasingSamples"].asInt();
		randomSamples = configData["randomSamples"].asInt();
	}

	printf("Working as Machine Id:%s\nisRenderMachine:%d\ntaskFetchFrequency:%d\nMAX_RAY_DEPTH:%d\nsamplesAO:%d\nminAOBrightness:%f\nantiAliasingSamples:%d\nrandomSamples:%d\n\n", machineId.c_str(), isRenderMachine, taskFetchFrequency, MAX_RAY_DEPTH, samplesAO, minAOBrightness, antiAliasingSamples, randomSamples);

	do {
		printf("Asking Server for new task\n");
		chdir(cCurrentPath);
		TaskDetails td = getTaskFromServer(machineId);

		if(td.taskId > 0) {
			printf("Task Assigned: %d Frame: %d Width: %d Height: %d\n\n", td.taskId, td.frame, td.width, td.height);
			timespec ts;
			clock_gettime(CLOCK_REALTIME, &ts);

			bool result = false;
			if(td.isRenderTask)
				result = renderTask(td);
			else
				result = videoTask(td);

			if(result) {
				timespec te;
				clock_gettime(CLOCK_REALTIME, &te);
				double realtime = (te.tv_sec - ts.tv_sec) + (double)(te.tv_nsec - ts.tv_nsec) / (double)1000000000.0;
				printf("Time taken: %.2fs\n\n", realtime);
			}
		} else {
			printf("No Pending Tasks waiting for %d seconds\n\n", taskFetchFrequency);
			sleep(taskFetchFrequency);
		}
	} while(true);

	return 0;
}


