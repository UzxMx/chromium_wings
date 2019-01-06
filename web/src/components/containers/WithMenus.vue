<template>
  <div class="app">
    <el-menu
      :default-active="menuDefaultActiveIndex"
      class="menu"
      background-color="#545c64"
      text-color="#fff"
      active-text-color="#ffd04b"
      ref="menu"
      :router="true">
      <div class="container-nav-back" @click="$router.go(-1)">
        <icon name="arrow-left" style="color: white;"></icon>
        <span>返回</span>
      </div>
      <el-submenu index="templates">
        <template slot="title">
          <i class="el-icon-location"></i>
          <span>模板管理</span>
        </template>
        <el-menu-item index="/templates/index">模板列表</el-menu-item>
        <el-menu-item index="/templates/new">添加模板</el-menu-item>
      </el-submenu>

      <el-submenu index="data">
        <template slot="title">
          <i class="el-icon-menu"></i>
          <span>数据管理</span>
        </template>
        <el-menu-item index="/data/index">数据列表</el-menu-item>
      </el-submenu>

      <el-submenu index="users">
        <template slot="title">
          <i class="el-icon-menu"></i>
          <span>用户管理</span>
        </template>
        <el-menu-item index="/users/index">用户列表</el-menu-item>
      </el-submenu>
    </el-menu>

    <main>
      <keep-alive>
        <router-view></router-view>
      </keep-alive>
    </main>
  </div>
</template>

<script>
import { mapGetters } from 'vuex'

export default {
  data () {
    return {
      menuDefaultActiveIndex: ''
    }
  },

  computed: mapGetters(['isAuthenticated']),

  mounted () {
    this.menuDefaultActiveIndex = this.$route.path
  }
}
</script>

<style lang="scss" scoped>
  .root {
    height: 100%;

    .app {
      height: 100%;
      display: flex;

      .menu {
        width: 250px;
        height: 100%;
      }

      main {
        flex: 1;
        overflow: auto;
      }    
    }

    .container-nav-back {
      padding: 15px 20px;
      color: white;
      display: flex;
      align-items: center;
      border-bottom: 1px solid #777;
      cursor: pointer;

      span {
        margin-left: 10px;
      }
    }    
  }
</style>
