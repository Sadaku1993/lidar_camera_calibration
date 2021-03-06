#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_cloud.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>
#include <iostream>

typedef pcl::PointXYZRGB PointA;
typedef pcl::PointCloud<PointA> CloudA;
typedef pcl::PointCloud<PointA>::Ptr CloudAPtr;

using namespace std;

ros::Publisher pub_ds_cloud;

double DS_SIZE = 0.01;
void down_sampling(CloudAPtr cloud, CloudAPtr& cloud_filtered, float size)
{
    // Create the filtering object
    pcl::VoxelGrid<PointA> sor;
    sor.setInputCloud (cloud);
    sor.setLeafSize (size, size, size);
    sor.filter (*cloud_filtered);
}

void pub_cloud(CloudAPtr cloud, 
               std_msgs::Header header, 
               ros::Publisher pub)
{
    sensor_msgs::PointCloud2 output;
    pcl::toROSMsg(*cloud, output);
    output.header.stamp = header.stamp;
    output.header.frame_id = header.frame_id;
    pub.publish(output);
}

void pcCallback(const sensor_msgs::PointCloud2ConstPtr msg)
{
    CloudAPtr cloud(new CloudA);
    pcl::fromROSMsg(*msg, *cloud);

    CloudAPtr ds_cloud(new CloudA);
    down_sampling(cloud, ds_cloud, DS_SIZE);

    pub_cloud(ds_cloud, msg->header, pub_ds_cloud);
}


int main(int argc, char**argv)
{
    ros::init(argc, argv, "camera_downsample");
    ros::NodeHandle n;
    ros::NodeHandle nh("~");

    nh.getParam("ds_size", DS_SIZE);

    ros::Subscriber sub = n.subscribe("/cloud", 10, pcCallback);
    pub_ds_cloud = n.advertise<sensor_msgs::PointCloud2>("/output", 10);

    ros::spin();

    return 0;
}
