#include <dlib/clustering.h>
#include <dlib/matrix.h>
#include <plot.h>

#include <experimental/filesystem>
#include <iostream>
#include <unordered_map>

using namespace dlib;
namespace fs = std::experimental::filesystem;
using SampleType = dlib::matrix<double, 1, 1>;
using Samples = std::vector<SampleType>;

const std::vector<std::string> data_names{"dataset0.csv", "dataset1.csv",
                                          "dataset2.csv", "dataset3.csv",
                                          "dataset4.csv", "dataset5.csv"};

const std::vector<std::string> colors{"black", "red",    "blue",  "green",
                                      "cyan",  "yellow", "brown", "magenta"};

using DataType = double;
using Coords = std::vector<DataType>;
using PointCoords = std::pair<Coords, Coords>;
using Clusters = std::unordered_map<size_t, PointCoords>;

void PlotClusters(const Clusters& clusters,
                  const std::string& name,
                  const std::string& file_name) {
  plotcpp::Plot plt;
  plt.SetTerminal("png");
  plt.SetOutput(file_name);
  plt.SetTitle(name);
  plt.SetXLabel("x");
  plt.SetYLabel("y");
  plt.SetAutoscale();
  plt.GnuplotCommand("set grid");

  auto draw_state = plt.StartDraw2D<Coords::const_iterator>();
  for (auto& cluster : clusters) {
    std::stringstream params;
    params << "lc rgb '" << colors[cluster.first] << "' pt 7";
    plt.AddDrawing(draw_state,
                   plotcpp::Points(
                       cluster.second.first.begin(), cluster.second.first.end(),
                       cluster.second.second.begin(),
                       std::to_string(cluster.first) + " cls", params.str()));
  }

  plt.EndDraw2D(draw_state);
  plt.Flush();
}

template <typename I>
void DoHierarhicalClustring(const I& inputs,
                            size_t num_clusters,
                            const std::string& name)
{
    matrix<double> dists(inputs.nr(), inputs.nr());
    for(long r = 0; r < dists.nr(); r++)
        for(long c = 0; c < dists.nc(); c++)
            dists(r, c) = length(subm(inputs, r, 0, 1, 2) - subm(inputs, c, 0, 1, 2));

    std::vector<unsigned long> clusters;
    bottom_up_cluster(dists, clusters, num_clusters);
    Clusters plot_clusters;
    for(long i = 0; i != inputs.nr(); i++)
    {
        auto cluster_idx = clusters[i];
        plot_clusters[cluster_idx].first.push_back(inputs(i, 0));
        plot_clusters[cluster_idx].second.push_back(inputs(i, 1));
    }

    PlotClusters(plot_clusters, "Agglomerative clustring" , name + "-aggl.png");
}


template <typename I>
void DoGraphNewmanClustring(const I& inputs, const std::string& name)
{
    std::vector<sample_pair> edges;
    for(long i = 0; i <inputs.nr(); i++)
        for(long j = 0; j < inputs.nr(); j++)
        {
            auto dist = length(subm(inputs, i, 0, 1, 2) - subm(inputs, j, 0, 1, 2));
            if (dist < 0.5)
                edges.push_back(sample_pair(i, j, dist));
        }
    remove_duplicate_edges(edges);

    std::vector<unsigned long> clusters;
    const auto num_clusters = newman_cluster(edges, clusters);
    std::cout << "num_clusters detected: " << num_clusters << std::endl;
    Clusters plot_clusters;
    for(long i = 0; i not_eq inputs.nr(); i++)
    {
        auto cluster_idx = clusters[i];
        plot_clusters[cluster_idx].first.push_back(inputs(i, 0));
        plot_clusters[cluster_idx].second.push_back(inputs(i, 1));
    }

    PlotClusters(plot_clusters, "Graph Newman clustering",
                 name + "-graph-newman.png");
}

int main()
{
    auto base_dir = fs::path("/home/navid/Downloads/Hands-On-Machine-Learning-with-CPP-master/Chapter04/data");
    for(auto& dataset : data_names)
    {
        auto dataset_name = base_dir / dataset;
        if(fs::exists(dataset_name))
        {
            std::ifstream file(dataset_name);
            matrix<DataType> data;
            file >> data;

            auto inputs = dlib::subm(data, 0, 1, data.nr(), 2);
            auto labels = dlib::subm(data, 0, 3, data.nr(), 1);
            auto num_samples = inputs.nr();
            auto num_features = inputs.nc();
            std::size_t num_clusters =
                    std::set<double>(labels.begin(), labels.end()).size();
            if(num_clusters < 2)
                num_clusters = 3;
            std::cout << dataset << std::endl
                      << "num samples: " << num_samples << std::endl
                      << "num featurs: " << num_features << std::endl
                      << "num clusters: " << num_clusters << std::endl;

//            DoHierarhicalClustring(inputs, num_clusters, dataset);
            DoGraphNewmanClustring(inputs, dataset);

        }
    }
}
