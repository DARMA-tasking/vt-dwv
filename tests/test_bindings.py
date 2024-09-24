"""TEST BINDINGS"""

import os
import json
import sys
import yaml

import vttv

# source dir is the directory a level above this file
source_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# Read the YAML config file
with open(f'{source_dir}/tests/test_bindings_conf.yaml', 'r', encoding="UTF-8") as stream:
    try:
        params = yaml.safe_load(stream)
    except yaml.YAMLError as exc:
        print(exc)

# Check main key is "visualization"
if "visualization" not in params:
    print("The YAML configuration file is not valid: missing required paramaters: visualization")
    sys.exit(1)

# make output_visualization_dir directory parameter absolute
if "output_visualization_dir" in params["visualization"]:
    params["visualization"]["output_visualization_dir"] = os.path.abspath(
      params["visualization"]["output_visualization_dir"]
    )

params_serialized = yaml.dump(params["visualization"])

n_ranks = params["visualization"]["x_ranks"]
n_ranks *= params["visualization"]["y_ranks"]
n_ranks *= params["visualization"]["z_ranks"]

# Prepare rank data
rank_data = []
for rank in range(n_ranks):
    with open(f'{source_dir}/data/lb_test_data/data.{rank}.json', 'r', encoding="UTF-8") as f:
        data = json.load(f)

    data_serialized = json.dumps(data)
    rank_data.append((data_serialized))

# Launch VT TV from JSON data
vttv.tvFromJson(rank_data, params_serialized, n_ranks)
