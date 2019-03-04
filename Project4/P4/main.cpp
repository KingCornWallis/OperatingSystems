//
//  main.cpp
//  MemoryManagement
//
//  Created by Jacob Darabaris on 4/13/18.
//  Copyright © 2018 Jacob Darabaris. All rights reserved.
//


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <algorithm>
using namespace std;

int PID_COUNT = 150;
int PAGE_REF_COUNT = 10000;

int ReferenceHandler(int, int);
int FIFOtest(vector <int>, vector <int>, vector <int>);
int FIFOalgorithm(vector<int>, int);
int LRUtest(vector <int>, vector <int>, vector <int>);
int LRUalgorithm(vector <int>, int);
int OPTtest(vector <int>, vector<int>, vector<int>);
int OPTalgorithm(vector<int>, int);

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout << "You need an input file and an integer to run " << argv[0] << endl;
        return 1;
    }
    
    ifstream inputFile(argv[1]);
    if(!inputFile)
    {
        cout << "Unable to open file: " << argv[1] << endl;
        return 2;
    }
    
    int frameCount = atoi(argv[2]);
    if(!frameCount)
    {
        cout << "This is not a valid positive number: " << argv[2] << endl;
        return 3;
    }
    
    vector<int> pageRefArray(PAGE_REF_COUNT);
    vector<int> frameBuffer(frameCount, 0);
    vector<int> frameBuffer2(frameBuffer);
    vector<int> frameBuffer3(frameBuffer);
    vector<int> counter(frameBuffer);
    vector<int> counter2(frameBuffer);
    vector<int> counter3(frameBuffer);
    int PID, pageRef;
    int pageRefIndex = 0;
    int faults = 0;
    
    while(inputFile >> PID >> pageRef)
    {
        int newPageRef = ReferenceHandler(PID, pageRef);
        pageRefArray[pageRefIndex] = newPageRef;
        pageRefIndex++;
    }
    //pageRefArray.resize(pageRefIndex);
    
    faults = FIFOtest(pageRefArray, frameBuffer, counter);
    cout << "FIFO faults: " << faults << endl;
    faults = LRUtest(pageRefArray, frameBuffer2, counter2);
    cout << "LRU faults: " << faults << endl;
    faults = OPTtest(pageRefArray, frameBuffer3, counter3);
    cout << "OPT faults: " << faults << endl;
    return 0;
}

int ReferenceHandler(int PID, int pageRef)
{
    int uniquePageRef = PID*10000 + pageRef + 1;
    return uniquePageRef;
}

int FIFOtest(vector <int> pageRefArray, vector <int> frameBuffer, vector <int> counter)
{
    //cout << "Beginning FIFO algorithm" << endl;
    int FIFOfaults = 0;
    for (int i = 0; i < pageRefArray.size(); i++)
    {
        bool passComplete = false;
        
        for (int j = 0; j < frameBuffer.size(); j++)
        {
            if(frameBuffer[j] == pageRefArray[i])
            {
                //cout << pageRefArray[i] - 1 << " was found in the frame buffer at index: " << j << endl;
                passComplete = true;
                break;
            }
        }
        //auto
        
        if(!passComplete)
        {
            FIFOfaults++;
            
            for (int j = 0; j < frameBuffer.size(); j++)
            {
                if(frameBuffer[j] == 0)
                {
                    frameBuffer[j] = pageRefArray[i];
                    counter[j]++;
                    //cout << pageRefArray[i] - 1 << " was placed in an empty frame at index: " << j << endl;
                    passComplete = true;
                    break;
                }
            }
        }
        
        if (!passComplete)
        {
            
            int targetFrame = FIFOalgorithm(counter, frameBuffer.size());
            //int oldFrame = frameBuffer[targetFrame];
            frameBuffer[targetFrame] = pageRefArray[i];
            //cout << pageRefArray[i] - 1 << " was swapped with " << oldFrame - 1 << " at index: " << targetFrame << endl;
            counter[targetFrame] = 1;
            for (int k = 0; k < frameBuffer.size(); k++)
            {
                if (k != targetFrame)
                    counter[k]++;
            }
        }
    }
    //cout << "End of FIFO algorithm" << endl;
    return FIFOfaults;
}

int FIFOalgorithm(vector <int> counter, int frameCount)
{
    int max = counter[0];
    int frame = 0;
    for(int i = 0; i < frameCount; i++)
    {
        if (counter[i] > max)
        {
            frame = i;
            max = counter[i];
        }
    }
    return frame;
}

int LRUtest(vector <int> pageRefArray, vector <int> frameBuffer, vector <int> counter)
{
    //cout << "Beginning LRU algorithm" << endl;
    int LRUfaults = 0;
    int recentlyUsed = 0;
    for (int i = 0; i < pageRefArray.size(); i++)
    {
        bool passComplete = false;
        
        for(int j = 0; j < frameBuffer.size(); j++)
        {
            if (frameBuffer[j] == pageRefArray[i])
            {
                recentlyUsed++;
                counter[j] = recentlyUsed;
                //cout << pageRefArray[i] - 1 << " was found in the frame buffer at index: " << j << endl;
                passComplete = true;
                break;
            }
        }
        
        if(!passComplete)
        {
            LRUfaults++;
            for (int j = 0; j < frameBuffer.size(); j++)
            {
                if(frameBuffer[j] == 0)
                {
                    frameBuffer[j] = pageRefArray[i];
                    recentlyUsed++;
                    counter[j] = recentlyUsed;
                    //cout << pageRefArray[i] - 1 << " was placed in an empty frame at index: " << j << endl;
                    passComplete = true;
                    break;
                }
            }
        }
        if (!passComplete)
        {
            int targetFrame = LRUalgorithm(counter, frameBuffer.size());
            //int oldFrame = frameBuffer[targetFrame];
            frameBuffer[targetFrame] = pageRefArray[i];
            //cout << pageRefArray[i] - 1 << " was swapped with " << oldFrame - 1 << " at index: " << targetFrame << endl;
            recentlyUsed++;
            counter[targetFrame] = recentlyUsed;
        }
    }
    //cout << "End of LRU algorithm" << endl;
    return LRUfaults;
}

int LRUalgorithm(vector <int> counter, int frameCount)
{
    int min = counter[0];
    int frame = 0;
    for(int i = 0; i < frameCount; i++)
    {
        if (counter[i] < min)
        {
            min = counter[i];
            frame = i;
        }
    }
    return frame;
}

int OPTtest(vector <int> pageRefArray, vector <int> frameBuffer, vector <int> counter)
{
    //cout << "Beginning OPT algorithm" << endl;
    int OPTfaults = 0;
    for (int i = 0; i < pageRefArray.size(); i++)
    {
        bool passComplete = false;
        
        for (int j = 0; j < frameBuffer.size(); j++)
        {
            if (frameBuffer[j] == pageRefArray[i])
            {
                //cout << pageRefArray[i] - 1 << " was found in the frame buffer at index: " << j << endl;
                passComplete = true;
                break;
            }
        }
        if(!passComplete)
        {
            OPTfaults++;
            for (int j = 0; j < frameBuffer.size(); j++)
            {
                if(frameBuffer[j] == 0)
                {
                    frameBuffer[j] = pageRefArray[i];
                    //cout << pageRefArray[i] - 1 << " was placed in an empty frame at index: " << j << endl;
                    passComplete = true;
                    break;
                }
            }
            
            for(int k = 0; k < frameBuffer.size(); k++)
            {
                counter[k] = 0; //refresh counter
                for(int g = i + 1; g < pageRefArray.size(); g++)
                {
                    if(frameBuffer[k] == pageRefArray[g])
                    {
                        counter[k] = g;
                        break;
                    }
                }
            }
            
             
        }
        if(!passComplete)
        {
            int targetFrame = OPTalgorithm(counter, frameBuffer.size());
            //int oldFrame = frameBuffer[targetFrame];
            frameBuffer[targetFrame] = pageRefArray[i];
            //cout << pageRefArray[i] - 1 << " was swapped with " << oldFrame - 1 << " at index: " << targetFrame << endl;
        }
        
        
    }
    //cout << "End of OPT algorithm" << endl;
    return OPTfaults;
}
int OPTalgorithm(vector <int> counter, int frameCount)
{
    
    int max = counter[0];
    int frame = 0;
    int flag = 0;
    for(int j = 0; j < frameCount; j++)
    {
        if(counter[j] == 0)
        {
            frame = j;
            flag = 1;
            break;
        }
    }
    if (flag == 0)
    {
    for(int i = 1; i < frameCount; i++)
    {
        if(max < counter[i])
        {
            frame = i;
            max = counter[i];
        }
    }
    }
    return frame;
}
