
#include "controllers/map-controller.h"

#include <utility>
#include <vector>
#include "base/debug.h"
#include "engine/game.h"


void MapController::Load(unsigned width, unsigned height) {
  width_ = width;
  height_ = height;
  towers_.resize(width * height, nullptr);
  enemies_.resize(width * height);
}


void MapController::Step(sf::Time elapsed) {
}


bool MapController::ReachedCastle(Vec2u tile) {
  return (tile.x < 2);
}


void MapController::EventTriggered(GameEvent event) {
}


void MapController::PlaceEnemy(Vec2u tile, EnemyUnit* enemy) {
  unsigned n = Index_(tile);

  auto pos = enemy_position_.find(enemy);
  if (pos != enemy_position_.end()) {
    if (pos->second == n) {
      return;
    }

    auto removed = std::remove(enemies_[pos->second].begin(),
                               enemies_[pos->second].end(), enemy);
    enemies_[pos->second].erase(removed, enemies_[pos->second].end());
  }

  enemies_[n].push_back(enemy);
  enemy_position_[enemy] = Index_(tile);
}

void MapController::RemoveEnemy(EnemyUnit* enemy) {
  auto pos = enemy_position_.find(enemy);
  if (pos == enemy_position_.end()) {
    return;
  }

  auto removed = std::remove(enemies_[pos->second].begin(),
                             enemies_[pos->second].end(), enemy);
  enemies_[pos->second].erase(removed, enemies_[pos->second].end());

  enemy_position_.erase(pos);
}


bool MapController::CanPlaceTower(Vec2u tile) {
  if (tile.x < 2 || tile.x > width_ - 3) {
    return false;
  }

  if (enemies_[Index_(tile)].size() > 0) {
    return false;
  }

  auto player_controller = Game::GetPlayerController();
  if (player_controller->gold() < kTowerCost) {
    return false;
  }

  unsigned min_row = tile.y;
  if (min_row > 0) {
    min_row--;
  }
  unsigned max_row = tile.y;
  if (tile.y < height_ - 1) {
    max_row++;
  }
  unsigned min_col = tile.x - 1;
  unsigned max_col = tile.x + 1;

  for (unsigned i = min_row; i <= max_row; ++i) {
    for (unsigned j = min_col; j <= max_col; ++j) {
      if (towers_[Index_(Vec2u(j, i))] != nullptr) {
        return false;
      }
    }
  }

  return true;
}



std::vector<EnemyUnit *> MapController::GetNearEnemies(Vec2u tile) {
  std::vector<EnemyUnit *> near_enemies;

  unsigned min_row = tile.y;
  if (min_row > 0) {
    min_row--;
  }
  unsigned max_row = tile.y;
  if (tile.y < height_ - 1) {
    max_row++;
  }
  unsigned min_col = tile.x - 1;
  unsigned max_col = tile.x + 1;

  for (unsigned i = min_row; i <= max_row; ++i) {
    for (unsigned j = min_col; j <= max_col; ++j) {
      for (EnemyUnit * enemy : enemies_[Index_(Vec2u(j, i))]) {
        near_enemies.push_back(enemy);
      }
    }
  }

  return near_enemies;
}


void MapController::PlaceTower(Vec2u tile, Tower * tower) {
  towers_[Index_(tile)] = tower;
}


unsigned MapController::Index_(Vec2u tile) {
  return tile.y * width_ + tile.x;
}

Vec2u MapController::CalcRowCol(Vec2f position) {
  sf::Vector2f tile_size(Game::GetTileSize());

  unsigned row = static_cast<unsigned>(position.y / tile_size.y);
  unsigned col = static_cast<unsigned>(position.x / tile_size.x);

  return Vec2u(col, row);
}

