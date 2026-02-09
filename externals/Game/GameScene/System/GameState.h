#pragma once

// 名前の衝突を防ぐために enum にスコープを付与
enum class State {
	kGame,
	kPause,
};

enum class MenuCommand {
	kReturnToGame,
	kRestart,
	kReturnToStageSelect,
	kTOBUZO,
};