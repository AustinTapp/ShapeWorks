
#include "DeepSSMParameters.h"

namespace shapeworks {

const std::string DeepSSMParameters::DEEPSSM_SAMPLER_GAUSSIAN_C("Gaussian");
const std::string DeepSSMParameters::DEEPSSM_SAMPLER_MIXTURE_C("Mixture");
const std::string DeepSSMParameters::DEEPSSM_SAMPLER_KDE_C("KDE");

namespace Keys {
const std::string AUG_NUM_SAMPLES = "aug_num_samples";
const std::string AUG_NUM_DIMS = "aug_num_dims";
const std::string AUG_PERCENT_VARIABILITY = "aug_percent_variability";
const std::string AUG_SAMPLER_TYPE = "aug_sampler_type";

const std::string TRAIN_EPOCHS = "train_epochs";
const std::string TRAIN_LEARNING_RATE = "train_learning_rate";
const std::string TRAIN_DECAY_LEARNING_RATE = "train_decay_learning_rate";
const std::string TRAIN_FINE_TUNING = "train_fine_tuning";

}

namespace Defaults {

// augmentation defaults
const int AUG_NUM_SAMPLES = 3;
const int AUG_NUM_DIMS = 3;
double AUG_PERCENT_VARIABILITY = 0.95;
std::string AUG_SAMPLER_TYPE = DeepSSMParameters::DEEPSSM_SAMPLER_GAUSSIAN_C;

// training defaults
int TRAIN_EPOCHS = 100;
double TRAIN_LEARNING_RATE = 0.001;
bool TRAIN_DECAY_LEARNING_RATE = true;
bool TRAIN_FINE_TUNING = true;
}

//---------------------------------------------------------------------------
DeepSSMParameters::DeepSSMParameters(ProjectHandle project) : project_(project)
{
  this->params_ = this->project_->get_parameters(Parameters::DEEPSSM_PARAMS);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::restore_defaults()
{
  this->params_.reset_parameters();
}

//---------------------------------------------------------------------------
void DeepSSMParameters::save_to_project()
{
  this->project_->set_parameters(Parameters::DEEPSSM_PARAMS, this->params_);
}

//---------------------------------------------------------------------------
int DeepSSMParameters::get_aug_num_samples()
{
  return this->params_.get(Keys::AUG_NUM_SAMPLES, Defaults::AUG_NUM_SAMPLES);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_aug_num_samples(int num_samples)
{
  this->params_.set(Keys::AUG_NUM_SAMPLES, num_samples);
}

//---------------------------------------------------------------------------
int DeepSSMParameters::get_aug_num_dims()
{
  return this->params_.get(Keys::AUG_NUM_DIMS, Defaults::AUG_NUM_DIMS);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_aug_num_dims(int num_dims)
{
  this->params_.set(Keys::AUG_NUM_DIMS, num_dims);
}

//---------------------------------------------------------------------------
double DeepSSMParameters::get_aug_percent_variability()
{
  return this->params_.get(Keys::AUG_PERCENT_VARIABILITY, Defaults::AUG_PERCENT_VARIABILITY);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_aug_percent_variability(double percent_variability)
{
  this->params_.set(Keys::AUG_PERCENT_VARIABILITY, percent_variability);
}

//---------------------------------------------------------------------------
std::string DeepSSMParameters::get_aug_sampler_type()
{
  return this->params_.get(Keys::AUG_SAMPLER_TYPE, Defaults::AUG_SAMPLER_TYPE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_aug_sampler_type(std::string sampler_type)
{
  this->params_.set(Keys::AUG_SAMPLER_TYPE, sampler_type);
}

//---------------------------------------------------------------------------
int DeepSSMParameters::get_training_epochs()
{
  return this->params_.get(Keys::TRAIN_LEARNING_RATE, Defaults::TRAIN_LEARNING_RATE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_epochs(int epochs)
{
  this->params_.set(Keys::TRAIN_LEARNING_RATE, epochs);
}

//---------------------------------------------------------------------------
double DeepSSMParameters::get_training_learning_rate()
{
  return this->params_.get(Keys::TRAIN_LEARNING_RATE, Defaults::TRAIN_LEARNING_RATE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_learning_rate(double rate)
{
  this->params_.set(Keys::TRAIN_LEARNING_RATE, rate);
}

//---------------------------------------------------------------------------
bool DeepSSMParameters::get_training_decay_learning_rate()
{
  return this->params_.get(Keys::TRAIN_DECAY_LEARNING_RATE, Defaults::TRAIN_DECAY_LEARNING_RATE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_decay_learning_rate(bool decay)
{
  this->params_.set(Keys::TRAIN_DECAY_LEARNING_RATE, decay);
}

//---------------------------------------------------------------------------
bool DeepSSMParameters::get_training_fine_tuning()
{
  return this->params_.get(Keys::TRAIN_FINE_TUNING, Defaults::TRAIN_FINE_TUNING);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_fine_tuning(bool fine_tuning)
{
  this->params_.set(Keys::TRAIN_FINE_TUNING, fine_tuning);
}


//---------------------------------------------------------------------------
void DeepSSMParameters::restore_augmentation_defaults()
{
  this->params_.remove_entry(Keys::AUG_NUM_SAMPLES);
  this->params_.remove_entry(Keys::AUG_NUM_DIMS);
  this->params_.remove_entry(Keys::AUG_PERCENT_VARIABILITY);
  this->params_.remove_entry(Keys::AUG_SAMPLER_TYPE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::restore_training_defaults()
{
  this->params_.remove_entry(Keys::TRAIN_EPOCHS);
  this->params_.remove_entry(Keys::TRAIN_LEARNING_RATE);
  this->params_.remove_entry(Keys::TRAIN_DECAY_LEARNING_RATE);
  this->params_.remove_entry(Keys::TRAIN_FINE_TUNING);
}

};