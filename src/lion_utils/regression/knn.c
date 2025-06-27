#include "knn.h"

#include <lion/sim.h>
#include <lion/vector.h>
#include <lion_sim/mem.h>
#include <lion_utils/vendor/log.h>
#include <math.h>

double euclidean_distance(lion_sim_t *sim, const lion_vector_t *x, const lion_vector_t *y) {
  // Assume both have same lengths and contain doubles
  double sum = 0.0;
  for (size_t i = 0; i < x->len; i++) {
    double diff  = lion_vector_get_d(sim, x, i) - lion_vector_get_d(sim, y, i);
    sum         += diff * diff;
  }
  return sqrt(sum);
}

int compare_neighbors(const void *a, const void *b) {
  const lion_knn_neighbor_t *na = a;
  const lion_knn_neighbor_t *nb = b;
  return (na->distance > nb->distance) - (na->distance < nb->distance);
}

lion_status_t lion_knn_regressor_init(lion_sim_t *sim, size_t n_neighbors, lion_knn_regressor_t *out) {
  lion_knn_regressor_t ret;
  ret.n_neighbors = n_neighbors;
  ret.is_trained  = 0;
  ret._neighbors  = NULL;

  *out = ret;
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_knn_regressor_cleanup(lion_sim_t *sim, lion_knn_regressor_t *knn) {
  if (knn->is_trained)
    lion_free(sim, knn->_neighbors);
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_knn_regressor_fit(lion_sim_t *sim, lion_knn_regressor_t *knn, lion_knn_sample_t *dataset, size_t n_samples) {
  if (knn->is_trained)
    logi_warn("Retraining already trained KNN regressor");

  // Setting up neighbors
  lion_knn_neighbor_t *neighbors = lion_malloc(sim, n_samples * sizeof(lion_knn_neighbor_t));
  knn->_dataset                  = dataset;
  knn->_neighbors                = neighbors;
  knn->_n_samples                = n_samples;

  knn->is_trained = 1;
  return LION_STATUS_SUCCESS;
}

double lion_knn_regressor_predict(lion_sim_t *sim, lion_knn_regressor_t *knn, lion_vector_t *X) {
  for (size_t i = 0; i < knn->_n_samples; i++) {
    knn->_neighbors[i].distance = euclidean_distance(sim, X, &knn->_dataset[i].X);
    knn->_neighbors[i].target   = knn->_dataset[i].y;
  }
  qsort(knn->_neighbors, knn->_n_samples, sizeof(lion_knn_neighbor_t), compare_neighbors);

  double sum = 0.0;
  for (size_t i = 0; i < knn->n_neighbors; i++) {
    sum += knn->_neighbors[i].target;
  }
  return sum / knn->n_neighbors;
}
