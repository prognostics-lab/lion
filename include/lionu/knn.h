#pragma once

#include <lion/status.h>
#include <lion/vector.h>
#include <stddef.h>

typedef struct lion_sim lion_sim_t;

typedef struct lion_knn_sample {
  lion_vector_t X;
  double        y;
} lion_knn_sample_t;

typedef struct lion_knn_neighbor {
  double distance;
  double target;
} lion_knn_neighbor_t;

typedef struct lion_knn_regressor {
  size_t               n_neighbors;
  int                  is_trained;
  lion_knn_sample_t   *_dataset;
  lion_knn_neighbor_t *_neighbors;
  size_t               _n_samples;
} lion_knn_regressor_t;

lion_status_t lion_knn_regressor_init(lion_sim_t *sim, size_t n_neighbors, lion_knn_regressor_t *out);
lion_status_t lion_knn_regressor_cleanup(lion_sim_t *sim, lion_knn_regressor_t *out);

lion_status_t lion_knn_regressor_fit(lion_sim_t *sim, lion_knn_regressor_t *knn, lion_knn_sample_t *dataset, size_t n_samples);
double        lion_knn_regressor_predict(lion_sim_t *sim, lion_knn_regressor_t *knn, lion_vector_t *X);
